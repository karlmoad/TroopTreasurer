#include "objects/accountbalancereportmodel.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlResult>
#include "objects/dates.h"
#include <QDebug>

namespace AccountBalanceReportSQL
{
    static const QString BalancesStmt = QString("SELECT AM.ACCT_KEY, SUM(J.ACTIVITY_VALUE) "
                                                    "FROM ACCOUNT_MASTER AM JOIN TROOP_TRACK_JOURNAL J ON AM.SOURCE_KEY = J.ACCT_HASH "
                                                    "WHERE AM.ACCT_PARENT IS NOT NULL AND J.ACTIVITY_DATE <= '%1' "
                                                    "GROUP BY AM.ACCT_KEY");
    static const QString AccountsStmt = QString("SELECT ACCT_KEY, ACCT_NAME, ACCT_PARENT, SOURCE_KEY, REPORTED_FLAG, ROLLUP_FLAG, EXTERNAL_FLAG FROM ACCOUNT_MASTER WHERE ACCT_PARENT IS NOT NULL ORDER BY ACCT_PARENT, DISPLAY_ORDER");

}

AccountBalanceReportModel::AccountBalanceReportModel(QObject *parent) :
            DataTreeModel("{00000000-0000-0000-0000-000000000000}","key", "parent" ,parent)
{}

AccountBalanceReportModel::~AccountBalanceReportModel()
{

}

void AccountBalanceReportModel::runReport()
{
    QSqlDatabase db = QSqlDatabase::database("DATABASE");
    QSqlQuery q(db);

    QString sql = AccountBalanceReportSQL::AccountsStmt;
    if(!q.exec(sql))
    {
        ObjectError err(q.lastError().databaseText(), static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
        err.raise();
        return;
    }

    beginResetModel();
    impl->addHeader("name","Account");
    QList<QJsonObject> data;
    while(q.next())
    {
        QJsonObject record;
        record["key"] = q.value(0).toString();
        record["name"] = q.value(1).toString();
        record["parent"] = q.value(2).toString();
        record["source"] = q.value(3).toString();
        record["reported"] = q.value(4).toInt() == 1;
        record["rollup"] = q.value(5).toInt() == 1;
        record["external"] = q.value(6).toInt() == 1;
        data.append(record);
    }
    impl->initialize(data);
    runDate(QDate::currentDate());
    endResetModel();
}

void AccountBalanceReportModel::addDate(const QDate &date)
{

}

void AccountBalanceReportModel::runDate(QDate date)
{
    QSqlDatabase db = QSqlDatabase::database("DATABASE");
    QSqlQuery q(db);

    QString d8 = QDate::currentDate().toString(DateFormats::DATABASE_FORMAT);
    QString sql = AccountBalanceReportSQL::BalancesStmt.arg(d8);
    if(!q.exec(sql))
    {
        ObjectError err(q.lastError().databaseText(), static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
        err.raise();
        return;
    }

    QMap<QString, QJsonObject> balanceData;
    while (q.next())
    {
        QJsonObject d;
        d["key"] = q.value(0).toString();
        d[d8] = q.value(1).toDouble();
        balanceData.insert(d["key"].toString(), d);
    }

    calcRollupBalances(impl->getRoot(), balanceData, d8);
    impl->addColumn(d8, d8, balanceData.values());
}

double AccountBalanceReportModel::calcRollupBalances(HierarchyItem *item, QMap<QString, QJsonObject> &data, const QString &fieldname)
{
    double balance = 0.0L;
    QJsonObject account = impl->getRecord(*(item));
    if(impl->isRoot(item) || !account.isEmpty())
    {
        double subTotals = 0.0l;
        if (item->subItemCount() > 0)
        {
            for (int i = 0; i < item->subItemCount(); i++)
            {
                subTotals += calcRollupBalances(item->subItem(i), data, fieldname);
            }
        }

        if (data.contains(item->id()))
        {
            if(account["source"].toString().trimmed().length() > 0)
            {
                balance = data[item->id()][fieldname].toDouble();
            }
        }
        else
        {
            balance = subTotals;
            QJsonObject rec;
            rec["key"] = item->id();
            rec[fieldname] = balance;
            data.insert(item->id(), rec);
        }
    }
    return balance;
}