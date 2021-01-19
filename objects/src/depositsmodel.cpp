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
    }
}

class Transactions::DepositsModel::DepositsModelImpl
{
    DepositsModelImpl()=default;
    ~DepositsModelImpl()=default;

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

Transactions::DepositsModel::DepositsModel(QObject *parent) : QAbstractTableModel(parent)
{
}

Transactions::DepositsModel::~DepositsModel()
{

}

int Transactions::DepositsModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}

int Transactions::DepositsModel::columnCount(const QModelIndex &parent) const
{
    return 0;
}

QVariant Transactions::DepositsModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

QVariant Transactions::DepositsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags Transactions::DepositsModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index);
}

void Transactions::DepositsModel::load(const QDate &begin, const QDate &end)
{

}

std::shared_ptr<Transactions::Deposit> Transactions::DepositsModel::getDeposit(const QModelIndex &index)
{
    return nullptr;
}

void Transactions::DepositsModel::addDeposit(const Transactions::Deposit &deposit)
{

}

void Transactions::DepositsModel::updateRecord(const QModelIndex &index)
{

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
