#include "objects/fundsrecordsmodel.h"
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
    namespace FundsSql
    {
        static const QString SelectOpenStmt = QString("SELECT * FROM FUNDS_REGISTER WHERE DEPOSIT_KEY IS NULL OR DEPOSIT_KEY='' ORDER BY FUNDS_DATE");
        static const QString SelectAllStmt = QString("SELECT * FROM FUNDS_REGISTER ORDER BY FUNDS_DATE");
        static const QString SelectByDepositStmt = QString("SELECT * FROM FUNDS_REGISTER WHERE DEPOSIT_KEY='%1'");
        static const QString DeleteStmt = QString("DELETE FROM FUNDS_REGISTER WHERE FUNDS_KEY='%1'");
        static const QString UpdateStmt = QString("UPDATE FUNDS_REGISTER SET %1 WHERE FUNDS_KEY='%2'");
        static const QString InsertStmt = QString("INSERT INTO FUNDS_REGISTER (%1) VALUES (%2)");
        static const QString ExistsStmt = QString("SELECT COUNT(*) FROM FUNDS_REGISTER WHERE FUNDS_KEY='%1'");
        static const QString DepositedCheckStmt = QString("SELECT DEPOSIT_KEY FROM FUNDS_REGISTER WHERE FUNDS_KEY='%1'");
    }
}

class Transactions::FundsRecordsModel::FundsRecordsModelImpl
{
public:
    FundsRecordsModelImpl()=default;
    ~FundsRecordsModelImpl()=default;

    void loadCurrentRecords()
    {
        _sqlStatement = Transactions::FundsSql::SelectOpenStmt;
        load();
    }

    void loadDepositRecords(const QString &deposit)
    {
        _sqlStatement = Transactions::FundsSql::SelectByDepositStmt.arg(deposit);
        load();
    }

    void loadAllRecords()
    {
        _sqlStatement = Transactions::FundsSql::SelectAllStmt;
        load();
    }

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

        if(!q.exec(_sqlStatement))
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

    bool recordModificationCheck(const QString &key, QString &message)
    {
        QString stmt = FundsSql::DepositedCheckStmt.arg(key);

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
                return query.value(0).toString().trimmed().length() == 0;
            }
        }
        message = "Record status is deposited, no modification permitted";
        return false;
    }

    static bool insertRecord(const FundsRecord &record, QString &message)
    {
        QString sql = FundsSql::InsertStmt;
        QString fields = "FUNDS_KEY,FUNDS_DATE,AMOUNT,METHOD,REF_VALUE,COMMENTS,WHO,WHAT,RECONCILED";
        QString buffer = QString("'%1'").arg(record.key());
        buffer.append(QString(",'%1'").arg(record.date().toString(DateFormats::DATABASE_FORMAT)));
        buffer.append(QString(",%1").arg(QString::number(record.amount(),'f',2)));
        buffer.append(QString(",%1").arg(QString::number(TransactionTypes::typeToInt(record.method()))));
        buffer.append(QString(",%1").arg((record.referenceValue().trimmed().length() > 0 ? QString("'%1'").arg(record.referenceValue().trimmed()) : "NULL")));
        buffer.append(QString(",%1").arg((record.comments().trimmed().length() > 0 ? QString("'%1'").arg(record.comments().trimmed()) : "NULL")));
        buffer.append(QString(",%1").arg((record.who().trimmed().length() > 0 ? QString("'%1'").arg(record.who().trimmed()) : "NULL")));
        buffer.append(QString(",%1").arg((record.what().trimmed().length() > 0 ? QString("'%1'").arg(record.what().trimmed()) : "NULL")));
        buffer.append(QString(",%1").arg(record.reconciled() ? "1" : "0"));

        QString stmt = sql.arg(fields,buffer);

        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        if(!db.open())
        {
            message = QString("Insert record failed: Database failed to open, " + QString(db.lastError().databaseText()));
            return false;
        }

        QSqlQuery insert(db);
        if(!insert.exec(stmt))
        {
            message = QString("Insert record failed: " + insert.lastError().text() + "- SQL:" + sql);
            return false;
        }
        else
        {
            return true;
        }
    }

    static bool updateRecord(const FundsRecord &record,QString &message)
    {
        QString buffer = QString("FUNDS_DATE = '%1'").arg(record.date().toString(DateFormats::DATABASE_FORMAT));
        buffer.append(QString(",AMOUNT=%1").arg(QString::number(record.amount(),'f',2)));
        buffer.append(QString(",METHOD=%1").arg(QString::number(TransactionTypes::typeToInt(record.method()))));
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

//        if(record.depositKey().trimmed().length() > 0)
//        {
//            buffer.append(QString("DEPOSIT_KEY='%1'").arg(record.depositKey().trimmed()));
//        }
//        else
//        {
//            buffer.append("DEPOSIT_KEY=NULL");
//        }

        QString stmt = FundsSql::UpdateStmt.arg(buffer, record.key());
        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        if(!db.open())
        {
            message = QString("Update record failed: Database failed to open, " + QString(db.lastError().databaseText()));
            return false;
        }

        QSqlQuery q(db);
        if(!q.exec(stmt))
        {
            message = QString("Update record failed: " + q.lastError().text() + "- SQL:" + stmt);
            return false;
        }
        else
        {
            return true;
        }
    }

    static bool deleteRecord(const FundsRecord &record,QString &message)
    {
        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        if(!db.open())
        {
            message = QString("Delete record failed: Database failed to open, " + QString(db.lastError().databaseText()));
            return false;
        }
        //make sure the record exists, if it doesnt add/update records could be in change queue only
        int existingCount = 0;
        QString existstmt = FundsSql::ExistsStmt.arg(record.key());
        QSqlQuery exists(db);
        if(!exists.exec(existstmt))
        {
            message = QString("Delete record failed: " + QString(exists.lastError().databaseText()) + "- existing record check failure");
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

        bool isDeposited = true; // err on side of caution

        //check to make sure the record is not part of a deposit, if it is it can not be deleted
        QString depositedstmt = FundsSql::DepositedCheckStmt.arg(record.key());
        QSqlQuery deposited(db);
        if(!deposited.exec(depositedstmt))
        {
            message = QString("Delete record failed: " + QString(exists.lastError().databaseText()) + "- deposited record check failure");
            return false;
        }

        if(deposited.first())
        {
            QString depositKey = deposited.value(0).toString().trimmed();
            if(depositKey.isEmpty() || depositKey.length() == 0)
            {
                isDeposited = false;
            }
        }

        if(isDeposited)
        {
            message = QString("Delete record failed: record is deposited");
            return false;
        }

        QString stmt = FundsSql::DeleteStmt.arg(record.key());

        QSqlQuery del(db);
        if(!del.exec(stmt))
        {
            message = QString("Delete record failed: " + QString(del.lastError().databaseText()) + "- SQL: " + stmt);
            return false;
        }
        else
        {
            return true;
        }
    }

    QString _sqlStatement;
    QList<std::shared_ptr<FundsRecord>> _records;
};

Transactions::FundsRecordsModel::FundsRecordsModel(QObject *parent) : QAbstractTableModel(parent), impl(new FundsRecordsModelImpl){}

Transactions::FundsRecordsModel::~FundsRecordsModel(){}

int Transactions::FundsRecordsModel::rowCount(const QModelIndex &parent) const
{
    return impl->_records.count();
}

int Transactions::FundsRecordsModel::columnCount(const QModelIndex &parent) const
{
    return 8;
}

QVariant Transactions::FundsRecordsModel::data(const QModelIndex &index, int role) const
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

QVariant Transactions::FundsRecordsModel::headerData(int section, Qt::Orientation orientation, int role) const
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

Qt::ItemFlags Transactions::FundsRecordsModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void Transactions::FundsRecordsModel::load(bool all)
{
    beginResetModel();
    if(all)
        impl->loadAllRecords();
    else
        impl->loadCurrentRecords();
    endResetModel();
}

void Transactions::FundsRecordsModel::load(const QString &deposit)
{
    beginResetModel();
    impl->loadDepositRecords(deposit);
    endResetModel();
}

std::shared_ptr<Transactions::FundsRecord> Transactions::FundsRecordsModel::getRecord(const QModelIndex &index)
{
    if(index.row() < impl->_records.count())
        return impl->_records.value(index.row());
    return nullptr;
}

void Transactions::FundsRecordsModel::addRecord(const Transactions::FundsRecord &record)
{
    QString msg;

    if(FundsRecordsModelImpl::insertRecord(record,msg))
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

void Transactions::FundsRecordsModel::updateRecord(const QModelIndex &index)
{
    QString msg;
    if(index.row() < impl->_records.count())
    {
        std::shared_ptr<FundsRecord> rec = impl->_records.value(index.row());
        if(impl->recordModificationCheck(rec->key(), msg))
        {
            if (FundsRecordsModelImpl::updateRecord(*(rec.get()), msg))
            {
                emit dataChanged(index, index);
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

void Transactions::FundsRecordsModel::deleteRecord(const QModelIndex &index)
{
    QString msg;
    if(index.row() < impl->_records.count())
    {
        std::shared_ptr<FundsRecord> rec = impl->_records.value(index.row());
        if(impl->recordModificationCheck(rec->key(), msg))
        {
            if (FundsRecordsModelImpl::deleteRecord(*(rec.get()), msg))
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

//Proxy

Transactions::FundsRecordProxyModel::FundsRecordProxyModel(QObject *parent) : QSortFilterProxyModel(parent){}

Transactions::FundsRecordProxyModel::~FundsRecordProxyModel(){}

bool Transactions::FundsRecordProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    return true;
}

bool Transactions::FundsRecordProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
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
