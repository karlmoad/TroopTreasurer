#ifndef TROOPTREASURER_DEPOSITSMODEL_H
#define TROOPTREASURER_DEPOSITSMODEL_H

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <memory>
#include "deposit.h"
#include "dates.h"
#include "objecterror.h"

namespace Transactions
{
    class DepositsModel: public  QAbstractTableModel
    {
    Q_OBJECT
    public:
        enum Roles
        {
            ObjectRole = Qt::UserRole + 1,
        };

        explicit DepositsModel(QObject *parent = nullptr);
        virtual ~DepositsModel();

        //QAbstractTableModel Overrides
        int rowCount(const QModelIndex &parent) const override;
        int columnCount(const QModelIndex &parent) const override;
        QVariant data(const QModelIndex &index, int role) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
        Qt::ItemFlags flags(const QModelIndex &index) const override;

        //Payments object functions
        void load(const QDate &begin = DateLimits::MinDate, const QDate &end = DateLimits::MaxDate);
        std::shared_ptr<Deposit> getDeposit(const QModelIndex& index);
        void addDeposit(const Deposit& deposit);
        void updateRecord(const QModelIndex& index);

    private:
            class DepositsModelImpl;
            std::shared_ptr<DepositsModelImpl> impl;
    };

    class DepositsProxyModel: public QSortFilterProxyModel
    {
    Q_OBJECT
    public:
        explicit DepositsProxyModel(QObject *parent= nullptr);
        virtual ~DepositsProxyModel();
        void setFinalizedStatusFilter(bool finalized);
    protected:
        virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
        virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
    private:
        class DepositsProxyModelImpl;
        std::shared_ptr<DepositsProxyModelImpl> impl;
    };
}


#endif //TROOPTREASURER_DEPOSITSMODEL_H
