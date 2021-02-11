#ifndef TROOPTREASURER_DATATABLEMODEL_H
#define TROOPTREASURER_DATATABLEMODEL_H

#include <QAbstractTableModel>
#include <memory>
#include "datatablemodelimpl.h"


class DataTableModel: public QAbstractTableModel
{
Q_OBJECT
public:
    explicit DataTableModel( QObject* parent= nullptr);
    virtual ~DataTableModel();
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual int columnCount(const QModelIndex &parent) const override;

protected:
    std::shared_ptr<DataTableModelImpl> impl;
};


#endif //TROOPTREASURER_DATATABLEMODEL_H
