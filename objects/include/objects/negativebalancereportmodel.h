#ifndef TROOPTREASURER_NEGATIVEBALANCEREPORTMODEL_H
#define TROOPTREASURER_NEGATIVEBALANCEREPORTMODEL_H

#include "datatablemodel.h"

class NegativeBalanceReportModel: public DataTableModel
{
    Q_OBJECT
public:
    explicit NegativeBalanceReportModel(QObject *parent= nullptr);
    virtual ~NegativeBalanceReportModel();

    void runReport();
};


#endif //TROOPTREASURER_NEGATIVEBALANCEREPORTMODEL_H
