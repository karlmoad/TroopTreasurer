#ifndef TROOPTREASURER_FUNDSRECORDSMODEL_H
#define TROOPTREASURER_FUNDSRECORDSMODEL_H

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <memory>
#include "fundsrecord.h"
#include "dates.h"
#include "objecterror.h"

namespace Transactions
{
    class FundsRecordsModel : public  QAbstractTableModel
    {
    Q_OBJECT
    public:
        enum Roles
        {
            ObjectRole = Qt::UserRole + 1,
        };

        explicit FundsRecordsModel(QObject *parent = nullptr);
        virtual ~FundsRecordsModel();

        //QAbstractTableModel Overrides
        int rowCount(const QModelIndex &parent) const override;
        int columnCount(const QModelIndex &parent) const override;
        QVariant data(const QModelIndex &index, int role) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
        Qt::ItemFlags flags(const QModelIndex &index) const override;

        //Payments object functions
        void load(bool all=false);
        void load(const QString& deposit);
        std::shared_ptr<FundsRecord> getRecord(const QModelIndex& index);
        void addRecord(const FundsRecord& record);
        void updateRecord(const QModelIndex& index);
        void deleteRecord(const QModelIndex& index);

    private:
        class FundsRecordsModelImpl;
        std::shared_ptr<FundsRecordsModelImpl> impl;
    };

    class FundsRecordProxyModel: public QSortFilterProxyModel
    {
    Q_OBJECT
    public:
        explicit FundsRecordProxyModel(QObject *parent= nullptr);
        virtual ~FundsRecordProxyModel();
    protected:
        virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
        virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
    };
}


#endif //TROOPTREASURER_FUNDSRECORDSMODEL_H
