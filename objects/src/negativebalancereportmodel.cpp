#include "objects/negativebalancereportmodel.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlResult>
#include "objects/objecterror.h"

namespace ReportSQl
{
    static const QString ReportQueryStmt = QString("SELECT A.ACCT_HASH , A.ACCT_NAME , MIN(A.ACTIVITY_DATE), TIMESTAMPDIFF(DAY,MIN(A.ACTIVITY_DATE),CURDATE()) AS DAYS_AGE "
                                                   "FROM TROOP_TRACK_JOURNAL A "
                                                   "JOIN (SELECT A2.ACCT_HASH, MAX(A2.ACTIVITY_DATE) AS LASTPOS "
                                                   "FROM TROOP_TRACK_JOURNAL A2 "
                                                   "WHERE A2.BALANCE >= 0 "
                                                   "GROUP BY A2.ACCT_HASH) AS LP ON A.ACCT_HASH = LP.ACCT_HASH "
                                                   "JOIN(SELECT AM.ACCT_KEY,AM.SOURCE_KEY,J.ACCT_HASH, SUM(J.ACTIVITY_VALUE) AS ACCT_BALANCE "
                                                   "FROM ACCOUNT_MASTER AM JOIN TROOP_TRACK_JOURNAL J ON AM.SOURCE_KEY = J.ACCT_HASH "
                                                   "WHERE AM.ACCT_PARENT IS NOT NULL "
                                                   "GROUP BY AM.ACCT_KEY,AM.SOURCE_KEY,J.ACCT_HASH) AS BAL ON A.ACCT_HASH = BAL.ACCT_HASH "
                                                   "WHERE A.ACTIVITY_DATE > LP.LASTPOS AND A.BALANCE < 0 AND BAL.ACCT_BALANCE < 0 "
                                                   "GROUP BY A.ACCT_HASH, A.ACCT_NAME ORDER BY A.ACCT_NAME");

}

NegativeBalanceReportModel::NegativeBalanceReportModel(QObject *parent) : DataTableModel(parent)
{
    impl->setHorizontalColumnsEnabled();
    impl->addHeader("name", "Account Name");
    impl->addHeader("date", "Date Negative");
    impl->addHeader("days", "Days Aging");
}

NegativeBalanceReportModel::~NegativeBalanceReportModel()
{

}

void NegativeBalanceReportModel::runReport()
{
    QSqlDatabase db = QSqlDatabase::database("DATABASE");
    QSqlQuery q(db);

    QString sql = ReportSQl::ReportQueryStmt;
    if(!q.exec(sql))
    {
        ObjectError err(q.lastError().databaseText(), static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
        err.raise();
        return;
    }

    beginResetModel();
    while(q.next())
    {
        QJsonObject record;
        record["name"] = q.value(1).toString();
        record["date"] = q.value(2).toString();
        record["days"] = q.value(3).toInt();
        record["acct_hash"] = q.value(0).toString();
        impl->addData(record);
    }
    endResetModel();
}
