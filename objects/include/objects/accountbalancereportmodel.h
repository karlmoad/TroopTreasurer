#ifndef TROOPTREASURER_ACCOUNTBALANCEREPORTMODEL_H
#define TROOPTREASURER_ACCOUNTBALANCEREPORTMODEL_H

#include <QDate>
#include "objects/datatreemodelcommon.h"
#include "objects/datatreemodel.h"
#include "objects/objecterror.h"

class AccountBalanceReportModel: public DataTreeModel
{
    Q_OBJECT
public:
    explicit AccountBalanceReportModel(QObject *parent= nullptr);
    virtual ~AccountBalanceReportModel();

    void runReport();
    void addDate(const QDate& date);

    virtual QVariant data(const QModelIndex &index, int role) const override;

    static int getUnassociatedAccountCount();

private:
    void runDate(QDate date, bool init=false);
    double calcRollupBalances(HierarchyItem* item, QMap<QString, QJsonObject> &data, const QString& fieldname);
};


#endif //TROOPTREASURER_ACCOUNTBALANCEREPORTMODEL_H
