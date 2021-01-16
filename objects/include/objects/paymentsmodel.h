#ifndef TROOPTREASURER_PAYMENTSMODEL_H
#define TROOPTREASURER_PAYMENTSMODEL_H

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <memory>
#include "payment.h"
#include "dates.h"
#include "objecterror.h"

namespace Transactions
{

    class PaymentsModel : public QAbstractTableModel
    {
    Q_OBJECT
    public:
        enum Roles
        {
            ObjectRole = Qt::UserRole + 1,
        };

        explicit PaymentsModel(QObject *parent = nullptr);
        virtual ~PaymentsModel();

        //QAbstractTableModel Overrides
        int rowCount(const QModelIndex &parent) const override;
        int columnCount(const QModelIndex &parent) const override;
        QVariant data(const QModelIndex &index, int role) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
        Qt::ItemFlags flags(const QModelIndex &index) const override;

        //Payments object functions
        void load(const QDate &begin = DateLimits::MinDate, const QDate &end = DateLimits::MaxDate);
        std::shared_ptr<Payment> getPayment(const QModelIndex& index);
        void addPayment(const Payment& payment);
        void updateRecord(const QModelIndex& index);
        void deleteRecord(const QModelIndex& index);
        void reset();
        QList<QString> save();

    signals:
        void pendingRecordChangesNotification(int count);


    private:
        class PaymentsModelImpl;
        std::shared_ptr<PaymentsModelImpl> impl;
    };

    class PaymentsProxyModel: public QSortFilterProxyModel
    {
    Q_OBJECT
    public:
        explicit PaymentsProxyModel(QObject *parent= nullptr);
        virtual ~PaymentsProxyModel();
        void setTransactionTypeFilter(TransactionTypes::Type type);
        void setFinalizedStatusFilter(bool finalized);
        void setActiveStatusFilter(bool active);
    protected:
        virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
        virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
    private:
        class PaymentsProxyModelImpl;
        std::shared_ptr<PaymentsProxyModelImpl> impl;
    };
}


#endif //TROOPTREASURER_PAYMENTSMODEL_H
