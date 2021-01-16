#include "objects/paymentsmodel.h"
#include <QList>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlResult>
#include "objects/change.h"
#include "objects/changequeue.h"
#include "objects/objecterror.h"
#include <QDebug>

namespace Transactions
{
    namespace PaymentsSql
    {
        static const QString SelectStmt = QString("SELECT * FROM PAYMENTS WHERE PAYMENT_DATE >= '%1' AND PAYMENT_DATE <= '%2' ORDER BY PAYMENT_DATE");
        static const QString DeleteStmt = QString("DELETE FROM PAYMENTS WHERE PAYMENT_KEY='%1'");
        static const QString UpdateStmt = QString("UPDATE PAYMENTS SET %1 WHERE PAYMENT_KEY='%2'");
        static const QString InsertStmt = QString("INSERT INTO PAYMENTS (%1) VALUES (%2)");
        static const QString ExistsStmt = QString("SELECT COUNT(*) FROM PAYMENTS WHERE PAYMENT_KEY='%1'");
    }
}

class Transactions::PaymentsModel::PaymentsModelImpl
{
public:
    PaymentsModelImpl(){}
    ~PaymentsModelImpl(){}

    void loadRecords(const QDate &begin, const QDate &end, QObject *parent= nullptr)
    {
        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        QSqlQuery q(db);

        QString sql = Transactions::PaymentsSql::SelectStmt.arg(begin.toString(DateFormats::DATABASE_FORMAT), end.toString(DateFormats::DATABASE_FORMAT));
        if(!q.exec(sql))
        {
            ObjectError err(q.lastError().databaseText(), static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
            err.raise();
            return;
        }

        while(q.next())
        {
            _payments.append(std::shared_ptr<Payment>(new Payment(q.record())));
        }
    }

    static bool insertPayment(const Payment& payment, QList<QString> &log)
    {
        QString sql = PaymentsSql::InsertStmt;
        QString fields = "PAYMENT_KEY,PAYMENT_DATE,AMOUNT,METHOD,REF_VALUE,COMMENTS,FINALIZED,FINALIZED_DATE,RECONCILED,WHO,WHAT";
        QString buffer = QString("'%1'").arg(payment.key());
        buffer.append(QString(",'%1'").arg(payment.date().toString(DateFormats::DATABASE_FORMAT)));
        buffer.append(QString(",%1").arg(QString::number(payment.amount(),'f',2)));
        buffer.append(QString(",%1").arg(QString::number(TransactionTypes::typeToInt(payment.method()))));
        buffer.append(QString(",%1").arg((payment.referenceValue().trimmed().length() > 0 ? QString("'%1'").arg(payment.referenceValue().trimmed()) : "NULL")));
        buffer.append(QString(",%1").arg((payment.comments().trimmed().length() > 0 ? QString("'%1'").arg(payment.comments().trimmed()) : "NULL")));
        buffer.append(QString(",%1").arg(payment.finalized() ? "1" : "0"));
        buffer.append(QString(",'%1'").arg(payment.finalizedDate().toString(DateFormats::DATABASE_FORMAT)));
        buffer.append(QString(",%1").arg(payment.reconciled() ? "1" : "0"));
        buffer.append(QString(",%1").arg((payment.who().trimmed().length() > 0 ? QString("'%1'").arg(payment.who().trimmed()) : "NULL")));
        buffer.append(QString(",%1").arg((payment.what().trimmed().length() > 0 ? QString("'%1'").arg(payment.what().trimmed()) : "NULL")));

        QString stmt = sql.arg(fields,buffer);

        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        QSqlQuery insert(db);
        if(!insert.exec(stmt))
        {
            log.append("Insert record failed: " + insert.lastError().text() + "- SQL:" + sql);
            return false;
        }
        else
        {
            return true;
        }
    }

    static bool updatePayment(const Payment &record, QList<QString> &log)
    {
        QString buffer = QString("REG_DATE = '%1'").arg(record.date().toString(DateFormats::DATABASE_FORMAT));
        buffer.append(QString(",AMOUNT=%1").arg(QString::number(record.amount(),'f',2)));
        buffer.append(QString(",PAY_METHOD=%1").arg(QString::number(TransactionTypes::typeToInt(record.method()))));
        buffer.append(QString(",FINALIZED=%1").arg(record.finalized() ? "1" : "0"));
        buffer.append(QString(",FINALIZED_DATE='%1'").arg(record.finalizedDate().toString(DateFormats::DATABASE_FORMAT)));
        buffer.append(QString(",RECONCILED=%1").arg(record.reconciled() ? "1" : "0"));
        if(record.referenceValue().trimmed().length() > 0)
        {
            buffer.append(QString(",REF_VALUE='%1'").arg(record.referenceValue().trimmed()));
        }
        if(record.comments().trimmed().length() > 0)
        {
            buffer.append(QString(",COMMENTS='%1'").arg(record.comments().trimmed()));
        }
        if(record.who().trimmed().length() > 0)
        {
            buffer.append(QString(",WHO='%1'").arg(record.who().trimmed()));
        }
        if(record.what().trimmed().length() > 0)
        {
            buffer.append(QString(",WHAT='%1'").arg(record.what().trimmed()));
        }

        QString stmt = PaymentsSql::UpdateStmt.arg(buffer, record.key());
        QSqlDatabase db = QSqlDatabase::database();
        QSqlQuery q(db);
        if(!q.exec(stmt))
        {
            log.append("Update record failed: " + q.lastError().text() + "- SQL:" + stmt);
            return false;
        }
        else
        {
            return true;
        }
    }

    static bool deletePayment(const Payment &record)
    {
        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        //make sure the record exists, if it doesnt add/update records could be in change queue only
        int existingCount = 0;
        QString existstmt = PaymentsSql::ExistsStmt.arg(record.key());
        QSqlQuery exists(db);
        if(!exists.exec(existstmt))
        {
            ObjectError err("Delete record failed: " + exists.lastError().text() + "- existing record check failure", static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
            err.raise();
            return false;
        }

        if(exists.first())
        {
            existingCount = exists.value(0).toInt();
        }

        if(existingCount == 0)
        {
            //if there is no record in the db it is essentially deleted,
            //return true so that table and change queue are purged of prior records
            return true;
        }

        QString stmt = PaymentsSql::DeleteStmt.arg(record.key());

        QSqlQuery del(db);
        if(!del.exec(stmt))
        {
            ObjectError err("Delete record failed: " + del.lastError().text() + "- SQL:" + stmt, static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
            err.raise();
            return false;
        }
        else
        {
            return true;
        }
    }

    QList<std::shared_ptr<Payment>> _payments;
    ChangeQueue<Payment> _changes;
};

Transactions::PaymentsModel::PaymentsModel(QObject *parent) : QAbstractTableModel(parent), impl(new PaymentsModelImpl)
{}

Transactions::PaymentsModel::~PaymentsModel()
{}

int Transactions::PaymentsModel::rowCount(const QModelIndex &parent) const
{
    return impl->_payments.count();
}

int Transactions::PaymentsModel::columnCount(const QModelIndex &parent) const
{
    return 9;
}

QVariant Transactions::PaymentsModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < impl->_payments.count())
    {
        std::shared_ptr<Payment> rec = impl->_payments[index.row()];

        if(role == Roles::ObjectRole)
        {
            return index.row(); //QVariant::fromValue<std::shared_ptr<Payment>>(rec);
        }
        else
        {
            switch (index.column())
            {
                case 0:
                {
                    switch (role)
                    {
                        case Qt::DisplayRole:
                            return rec->date().toString(DateFormats::UI_DISPLAY_FORMAT);
                        case Qt::UserRole:
                            return rec->date();
                        default:
                            return QVariant();
                    }
                }
                case 1:
                {
                    switch (role)
                    {
                        case Qt::DisplayRole:
                            return QString("$ %1").arg(QString::number(rec->amount(), 'f', 2));
                        case Qt::UserRole:
                            return rec->amount();
                        default:
                            return QVariant();
                    }
                }
                case 2:
                {
                    switch (role)
                    {
                        case Qt::DisplayRole:
                            return TransactionTypes::typeToString(rec->method());
                        case Qt::UserRole:
                            return TransactionTypes::typeToInt(rec->method());
                        default:
                            return QVariant();
                    }
                }
                case 3:
                {
                    switch (role)
                    {
                        case Qt::DisplayRole:
                        case Qt::UserRole:
                            return rec->referenceValue();
                        default:
                            return QVariant();
                    }
                }
                case 4:
                {
                    switch (role)
                    {
                        case Qt::DisplayRole:
                            return rec->reconciled() ? "Yes" : "No";
                        case Qt::UserRole:
                            return rec->reconciled();
                        default:
                            return QVariant();
                    }
                }
                case 5:
                {
                    switch (role)
                    {
                        case Qt::DisplayRole:
                            return (rec->finalized() ? rec->finalizedDate().toString(DateFormats::UI_DISPLAY_FORMAT) : "");
                        case Qt::UserRole:
                            return rec->finalizedDate();
                        default:
                            return QVariant();
                    }
                }
                case 6:
                {
                    switch (role)
                    {
                        case Qt::DisplayRole:
                        case Qt::UserRole:
                            return rec->who();
                        default:
                            return QVariant();
                    }
                }
                case 7:
                {
                    switch (role)
                    {
                        case Qt::DisplayRole:
                        case Qt::UserRole:
                            return rec->what();
                        default:
                            return QVariant();
                    }
                }
                case 8:
                {
                    switch (role)
                    {
                        case Qt::DisplayRole:
                        case Qt::UserRole:
                            return rec->comments();
                        default:
                            return QVariant();
                    }
                }
                default:
                    return QVariant();
            }
        }
    }
    return QVariant();
}

QVariant Transactions::PaymentsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( role != Qt::DisplayRole )
    {
        return QVariant();
    }

    switch(orientation)
    {
        case Qt::Horizontal:
        {
            if (section < columnCount(QModelIndex()))
            {
                switch (section)
                {
                    case 0:
                        return "Date";
                        break;
                    case 1:
                        return "Amount";
                        break;
                    case 2:
                        return "Method";
                        break;
                    case 3:
                        return "Ref";
                        break;
                    case 4:
                        return "Reconciled";
                        break;
                    case 5:
                        return "Finalized";
                        break;
                    case 6:
                        return "Who";
                        break;
                    case 7:
                        return "What";
                        break;
                    case 8:
                        return "Comments";
                        break;
                    default:
                        return QVariant();
                        break;
                }
            }
        }
        default:
            return QVariant();
    }
}

Qt::ItemFlags Transactions::PaymentsModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void Transactions::PaymentsModel::load(const QDate &begin, const QDate &end)
{
    beginResetModel();
    impl->_payments.clear();
    impl->loadRecords(begin,end, this);
    endResetModel();
}

QList<QString> Transactions::PaymentsModel::save()
{
    QList<QString> log;
    int depth = impl->_changes.depth();
    qDebug() << "Saving....";
    for(int i =0; i<depth; i++)
    {
        Change<Payment> mod = impl->_changes.pop();
        bool success = false;
        switch(mod.action())
        {
            case Action::CREATE:
            {
                qDebug() << "ADD Key: " << mod.object().key() << " Amount: " << mod.object().amount() << " What: " << mod.object().what();
                //success = PaymentsModelImpl::insertPayment(mod.object(), log);
                break;
            }
            case Action::UPDATE:
            {
                qDebug() << "Update Key: " << mod.object().key() << " Amount: " << mod.object().amount() << " What: " << mod.object().what();
                //success = PaymentsModelImpl::updatePayment(mod.object(), log);
                break;
            }
            // modifying delete to be non queued modification, delete will be carried out directly on delete call.
//            case Action::DELETE:
//            {
//                qDebug() << "Delete Key: " << mod.object().key() << " Amount: " << mod.object().amount() << " What: " << mod.object().what();
//                //success = PaymentsModelImpl::deletePayment(mod.object(),log);
//                break;
//            }
            default:
                break;
        }

        //requeue error records
        if(!success)
        {
            impl->_changes.push(mod);
        }
    }
    emit pendingRecordChangesNotification(impl->_changes.depth());
    return log;
}

std::shared_ptr<Transactions::Payment> Transactions::PaymentsModel::getPayment(const QModelIndex &index)
{
    if(index.row() < impl->_payments.count())
        return impl->_payments.value(index.row());
    return nullptr;
}

void Transactions::PaymentsModel::addPayment(const Payment& payment)
{
    int idx = rowCount(QModelIndex());
    beginInsertRows(QModelIndex(),idx,idx);
    std::shared_ptr<Payment> np = std::shared_ptr<Payment>(new Payment(payment));
    impl->_payments.append(np);
    Change<Payment> c;
    c.setReference(idx);
    c.setObject(payment);
    c.setAction(Action::CREATE);
    impl->_changes.push(c);
    endInsertRows();
    emit pendingRecordChangesNotification(impl->_changes.depth());
}

void Transactions::PaymentsModel::updateRecord(const QModelIndex &index)
{
    if(index.row() < impl->_payments.count())
    {
        Change<Payment> c;
        c.setAction(Action::UPDATE);
        c.setObject(*(impl->_payments[index.row()].get()));
        c.setReference(index.row());
        impl->_changes.push(c);
        emit dataChanged(index,index);
        emit pendingRecordChangesNotification(impl->_changes.depth());
    }
}

void Transactions::PaymentsModel::deleteRecord(const QModelIndex &index)
{
    if(index.row() < impl->_payments.count())
    {
        Payment p = *(impl->_payments.at(index.row()).get());
        bool success = PaymentsModelImpl::deletePayment(p);
        if(success)
        {
            impl->_changes.purgeRecordsFor(p);
            beginRemoveRows(QModelIndex(), index.row(), index.row());
            impl->_payments.removeAt(index.row());
            endRemoveRows();
            emit pendingRecordChangesNotification(impl->_changes.depth());
        }
    }
}

//sortfilterproxy

struct Transactions::PaymentsProxyModel::PaymentsProxyModelImpl
{
    TransactionTypes::Type _filterType = TransactionTypes::Type::UNKNOWN;
    bool _filterFinalized = false;
    bool _filterActive = false;
};



Transactions::PaymentsProxyModel::PaymentsProxyModel(QObject *parent) : QSortFilterProxyModel(parent), impl(new PaymentsProxyModelImpl){}

Transactions::PaymentsProxyModel::~PaymentsProxyModel(){}

void Transactions::PaymentsProxyModel::setFinalizedStatusFilter(bool finalized)
{
    impl->_filterFinalized = finalized;
    invalidateFilter();
}

void Transactions::PaymentsProxyModel::setActiveStatusFilter(bool active)
{
    impl->_filterActive = active;
    invalidateFilter();
}

void Transactions::PaymentsProxyModel::setTransactionTypeFilter(Transactions::TransactionTypes::Type type)
{
    impl->_filterType = type;
    invalidateFilter();
}

bool Transactions::PaymentsProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    PaymentsModel *pmodel = dynamic_cast<PaymentsModel*>(sourceModel());
    if(pmodel)
    {
        QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

        std::shared_ptr<Payment> rec = pmodel->getPayment(index);

        if (impl->_filterType != TransactionTypes::Type::UNKNOWN && rec->method() != impl->_filterType)
        {
            return false;
        }
        if (!impl->_filterFinalized && rec->finalized())
        {
            return false;
        }
        if (!impl->_filterActive && !rec->finalized())
        {
            return false;
        }
        return true;
    }
    return false;
}

bool Transactions::PaymentsProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    QVariant l = sourceModel()->data(source_left, Qt::UserRole);
    QVariant r = sourceModel()->data(source_right, Qt::UserRole);

    switch (l.userType())
    {
        case QMetaType::QDate:
            return l.toDate() < r.toDate();
        case QMetaType::Int:
            return l.toInt() < r.toInt();
        case QMetaType::Double:
            return l.toDouble() < r.toDouble();
        case QMetaType::QString:
            return l.toString().compare(r.toString(), Qt::CaseInsensitive) < 0;
        case QMetaType::Bool:
            return l.toBool() < r.toBool();
        default:
            return false;
    }
}


