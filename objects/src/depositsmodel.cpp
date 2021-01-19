#include "objects/depositsmodel.h"
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
    namespace DepositsSql
    {
        static const QString SelectStmt = QString("SELECT * FROM DEPOSITS WHERE DEPOSIT_DATE >= '%1' AND DEPOSIT_DATE <= '%2' ORDER BY DEPOSIT_DATE");
        static const QString UpdateStmt = QString("UPDATE DEPOSITS SET %1 WHERE DEPOSIT_KEY='%2'");
        static const QString InsertStmt = QString("INSERT INTO DEPOSITS (%1) VALUES (%2)");
        static const QString ExistsStmt = QString("SELECT COUNT(*) FROM DEPOSITS WHERE DEPOSIT_KEY='%1'");
        const QString FinalizedCheckStmt = QString("SELECT FINALIZED FROM DEPOSITS WHERE DEPOSIT_KEY='%1'");
    }
}

class Transactions::DepositsModel::DepositsModelImpl
{
public:
    DepositsModelImpl()=default;
    ~DepositsModelImpl()=default;

    void loadRecords(const QDate &begin, const QDate &end)
    {
        _begin = begin;
        _end = end;

        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        QSqlQuery q(db);

        QString sql = Transactions::DepositsSql::SelectStmt.arg(begin.toString(DateFormats::DATABASE_FORMAT), end.toString(DateFormats::DATABASE_FORMAT));
        if(!q.exec(sql))
        {
            ObjectError err(q.lastError().databaseText(), static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
            err.raise();
            return;
        }

        while(q.next())
        {
            _deposits.append(std::shared_ptr<Deposit>(new Deposit(q.record())));
        }
    }

    bool recordModificationCheck(const QString &key, QString &message)
    {
        QString stmt = DepositsSql::FinalizedCheckStmt.arg(key);

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
        message = "Record status is finalized, no modification permitted";
        return false;
    }

    static bool insertDeposit(const Deposit& deposit, QString &message)
    {
        QString sql = DepositsSql::InsertStmt;
        QString fields = "DEPOSIT_KEY,DEPOSIT_DATE,DEPOSIT_TOTAL,FINALIZED,FINALIZED_DATE,RECONCILED,WHO";
        QString buffer = QString("'%1'").arg(deposit.key());
        buffer.append(QString(",'%1'").arg(deposit.date().toString(DateFormats::DATABASE_FORMAT)));
        buffer.append(QString(",%1").arg(QString::number(deposit.total(),'f',2)));
        buffer.append(QString(",%1").arg(deposit.finalized() ? "1" : "0"));
        buffer.append(QString(",'%1'").arg(deposit.finalizedDate().toString(DateFormats::DATABASE_FORMAT)));
        buffer.append(QString(",%1").arg(deposit.reconciled() ? "1" : "0"));

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

    static bool updateDeposit(const Deposit& deposit, QString &message)
    {
        QString buffer = QString("DEPOSIT_DATE = '%1'").arg(deposit.date().toString(DateFormats::DATABASE_FORMAT));
        buffer.append(QString(",DEPOSIT_TOTAL=%1").arg(QString::number(deposit.total(),'f',2)));
        buffer.append(QString(",FINALIZED=%1").arg(deposit.finalized() ? "1" : "0"));
        buffer.append(QString(",FINALIZED_DATE='%1'").arg(deposit.finalizedDate().toString(DateFormats::DATABASE_FORMAT)));
        buffer.append(QString(",RECONCILED=%1").arg(deposit.reconciled() ? "1" : "0"));

        QString stmt = DepositsSql::UpdateStmt.arg(buffer, deposit.key());
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

    QList<std::shared_ptr<Deposit>> _deposits;
    QDate _begin;
    QDate _end;
};

Transactions::DepositsModel::DepositsModel(QObject *parent) : QAbstractTableModel(parent), impl(new DepositsModelImpl)
{}

Transactions::DepositsModel::~DepositsModel()
{

}

int Transactions::DepositsModel::rowCount(const QModelIndex &parent) const
{
    return impl->_deposits.count();
}

int Transactions::DepositsModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}

QVariant Transactions::DepositsModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < impl->_deposits.count())
    {
        std::shared_ptr<Deposit> rec = impl->_deposits[index.row()];

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
                            return QString("$ %1").arg(QString::number(rec->total(), 'f', 2));
                        case Qt::UserRole:
                            return rec->total();
                        default:
                            return QVariant();
                    }
                }
                case 2:
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
                case 3:
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
                default:
                    return QVariant();
            }
        }
    }
    return QVariant();
}

QVariant Transactions::DepositsModel::headerData(int section, Qt::Orientation orientation, int role) const
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
                        return "Total";
                        break;
                    case 2:
                        return "Reconciled";
                        break;
                    case 3:
                        return "Finalized";
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

Qt::ItemFlags Transactions::DepositsModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void Transactions::DepositsModel::load(const QDate &begin, const QDate &end)
{
    beginResetModel();
    impl->_deposits.clear();
    impl->loadRecords(begin,end);
    endResetModel();
}

std::shared_ptr<Transactions::Deposit> Transactions::DepositsModel::getDeposit(const QModelIndex &index)
{
    if(index.row() < impl->_deposits.count())
        return impl->_deposits.value(index.row());
    return nullptr;
}

void Transactions::DepositsModel::addDeposit(const Transactions::Deposit &deposit)
{
    QString msg;

    if(DepositsModelImpl::insertDeposit(deposit,msg))
    {
        int idx = rowCount(QModelIndex());
        beginInsertRows(QModelIndex(), idx, idx);
        std::shared_ptr<Deposit> np = std::shared_ptr<Deposit>(new Deposit(deposit));
        impl->_deposits.append(np);
        endInsertRows();
    }
    else
    {
        //reload from db to reset equal state
        load(impl->_begin, impl->_end);
        ObjectError err(msg, static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
        err.raise();
    }
}

void Transactions::DepositsModel::updateRecord(const QModelIndex &index)
{
    QString msg;
    if(index.row() < impl->_deposits.count())
    {
        std::shared_ptr<Deposit> rec = impl->_deposits.value(index.row());
        if(impl->recordModificationCheck(rec->key(), msg))
        {
            if (DepositsModelImpl::updateDeposit(*(rec.get()), msg))
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
    load(impl->_begin, impl->_end);
    ObjectError err(msg, static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
    err.raise();
}


// Proxy

Transactions::DepositsProxyModel::DepositsProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{

}

Transactions::DepositsProxyModel::~DepositsProxyModel()
{

}

void Transactions::DepositsProxyModel::setFinalizedStatusFilter(bool finalized)
{

}

bool Transactions::DepositsProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool Transactions::DepositsProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    return QSortFilterProxyModel::lessThan(source_left, source_right);
}
