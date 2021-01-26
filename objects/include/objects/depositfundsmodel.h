#ifndef TROOPTREASURER_DEPOSITFUNDSMODEL_H
#define TROOPTREASURER_DEPOSITFUNDSMODEL_H

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <memory>
#include "fundsrecord.h"
#include "deposit.h"
#include "dates.h"
#include "objecterror.h"

namespace Transactions
{
    class DepositFundsModel: public QAbstractTableModel
    {
        Q_OBJECT
    public:
        enum Roles
        {
            ObjectRole = Qt::UserRole + 1,
        };

        explicit DepositFundsModel(QObject *parent= nullptr);
        virtual ~DepositFundsModel();

        //QAbstractTableModel Overrides
        int rowCount(const QModelIndex &parent) const override;
        int columnCount(const QModelIndex &parent) const override;
        QVariant data(const QModelIndex &index, int role) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
        Qt::ItemFlags flags(const QModelIndex &index) const override;

        void addFundsRecord(FundsRecord record);
        void removeFundsRecord(const QModelIndex& index);
        void setDeposit(std::shared_ptr<Deposit> deposit);

    private:
        class DepositFundsModelImpl;
        std::shared_ptr<DepositFundsModelImpl> impl;
    };
}

#endif //TROOPTREASURER_DEPOSITFUNDSMODEL_H
