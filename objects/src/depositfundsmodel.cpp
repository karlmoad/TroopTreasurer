#include "objects/depositfundsmodel.h"
#include <QList>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlResult>
#include "objects/objecterror.h"
#include <QDebug>

namespace Transactions
{
    namespace DepositFundsSql
    {
        static const QString SelectStmt = QString("SELECT * FROM FUNDS_REGISTER WHERE DEPOSIT_KEY='%1'");
        static const QString UpdateStmt = QString("UPDATE FUNDS_REGISTER SET DEPOSIT_KEY='%1' WHERE FUNDS_KEY='%2'");
        static const QString RemoveStmt = QString("UPDATE FUNDS_REGISTER SET DEPOSIT_KEY=NULL WHERE FUNDS_KEY='%1'");
        static const QString CheckFinalizedStmt = QString("SELECT COUNT(*) FROM DEPOSITS WHERE DEPOSIT_KEY='%1' AND FINALIZED=1");
    }
}

class Transactions::DepositFundsModel::DepositFundsModelImpl
{
public:
    DepositFundsModelImpl()=default;
    ~DepositFundsModelImpl()=default;

    void load()
    {
        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        if(!db.open())
        {
            ObjectError err("Database failed to open, " + QString(db.lastError().databaseText()) ,static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
            err.raise();
            return;
        }

        QSqlQuery q(db);
        QString stmt = DepositFundsSql::SelectStmt.arg(_deposit->key());
        if(!q.exec(stmt))
        {
            ObjectError err(q.lastError().databaseText(), static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
            err.raise();
            return;
        }

        _records.clear();
        while(q.next())
        {
            _records.append(std::shared_ptr<FundsRecord>(new FundsRecord(q.record())));
        }
    }

    static bool recordModificationCheck(const QString& depositKey, QString &message)
    {
        QString stmt = DepositFundsSql::CheckFinalizedStmt.arg(depositKey);

        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        if(!db.open())
        {
            message = QString("Record check failed: Database failed to open, " + QString(db.lastError().databaseText()));
            return false;
        }

        QSqlQuery query(db);
        if(!query.exec(stmt))
        {
            message = QString("Record check failed: " + query.lastError().text() + "- SQL:" + stmt);
        }
        else
        {
            if(query.first())
            {
                return query.value(0).toInt() == 0;
            }
        }
        message = "Deposit finalized, no modification permitted";
        return false;
    }

    bool addDepositItem(const FundsRecord &record, QString &message)
    {
        if(recordModificationCheck(_deposit->key(), message))
        {
            QString stmt = DepositFundsSql::UpdateStmt.arg(_deposit->key(), record.key());
            QSqlDatabase db = QSqlDatabase::database("DATABASE");
            if(!db.open())
            {
                message = QString("Add deposit item: Database failed to open, " + QString(db.lastError().databaseText()));
                return false;
            }

            QSqlQuery query(db);
            if(!query.exec(stmt))
            {
                message = QString("Add deposit item: " + query.lastError().text() + "- SQL:" + stmt);
                return false;
            }
            return true;
        }
        return false;
    }

    bool removeDepositItem(const FundsRecord & record, QString& message)
    {
        if(recordModificationCheck(_deposit->key(), message))
        {
            QString stmt = DepositFundsSql::RemoveStmt.arg(record.key());
            QSqlDatabase db = QSqlDatabase::database("DATABASE");
            if(!db.open())
            {
                message = QString("Remove deposit item: Database failed to open, " + QString(db.lastError().databaseText()));
                return false;
            }

            QSqlQuery query(db);
            if(!query.exec(stmt))
            {
                message = QString("Remove deposit item: " + query.lastError().text() + "- SQL:" + stmt);
                return false;
            }
            return true;
        }
        return false;
    }

    std::shared_ptr<Deposit> _deposit;
    QList<std::shared_ptr<FundsRecord>> _records;
};

Transactions::DepositFundsModel::DepositFundsModel(QObject *parent) : QAbstractTableModel(parent), impl(new DepositFundsModelImpl){}

Transactions::DepositFundsModel::~DepositFundsModel(){}

int Transactions::DepositFundsModel::rowCount(const QModelIndex &parent) const
{
    return impl->_records.count();
}

int Transactions::DepositFundsModel::columnCount(const QModelIndex &parent) const
{
    return 8;
}

QVariant Transactions::DepositFundsModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < impl->_records.count())
    {
        std::shared_ptr<FundsRecord> rec = impl->_records[index.row()];

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
                        case Qt::UserRole:
                            return rec->who();
                        default:
                            return QVariant();
                    }
                }
                case 5:
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
                case 6:
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
                case 7:
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

QVariant Transactions::DepositFundsModel::headerData(int section, Qt::Orientation orientation, int role) const
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
                        return "Who";
                        break;
                    case 5:
                        return "What";
                        break;
                    case 6:
                        return "Reconciled";
                        break;
                    case 7:
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

Qt::ItemFlags Transactions::DepositFundsModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void Transactions::DepositFundsModel::setDeposit(std::shared_ptr<Deposit> deposit)
{
    beginResetModel();
    impl->_deposit=deposit;
    impl->load();
    endResetModel();
}

void Transactions::DepositFundsModel::addFundsRecord(Transactions::FundsRecord record)
{
    QString msg;
    record.setDepositKey(impl->_deposit->key());

    if(impl->addDepositItem(record, msg))
    {
        int idx = rowCount(QModelIndex());
        beginInsertRows(QModelIndex(), idx, idx);
        std::shared_ptr<FundsRecord> np = std::shared_ptr<FundsRecord>(new FundsRecord(record));
        impl->_records.append(np);
        endInsertRows();
    }
    else
    {
        //reload from db to reset equal state
        beginResetModel();
        impl->load();
        endResetModel();
        ObjectError err(msg, static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
        err.raise();
    }
}

void Transactions::DepositFundsModel::removeFundsRecord(const QModelIndex &index)
{
    QString msg;
    if(index.row() < impl->_records.count())
    {
        std::shared_ptr<FundsRecord> rec = impl->_records.value(index.row());
        rec->setDepositKey(QString());
        if(DepositFundsModelImpl::recordModificationCheck(impl->_deposit->key(), msg))
        {
            if (impl->removeDepositItem(*(rec.get()), msg))
            {
                beginRemoveRows(QModelIndex(), index.row(), index.row());
                impl->_records.removeAt(index.row());
                endRemoveRows();
                return;
            }
        }
    }
    else
    {
        msg = "invalid index";
    }

    //reload from db to reset equal state
    beginResetModel();
    impl->load();
    endResetModel();
    ObjectError err(msg, static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
    err.raise();
}
