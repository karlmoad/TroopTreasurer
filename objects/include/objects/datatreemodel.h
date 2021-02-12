#ifndef TROOPTREASURER_DATATREEMODEL_H
#define TROOPTREASURER_DATATREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QString>
#include <memory>
#include "datatreemodelcommon.h"

class DataTreeModel: public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit DataTreeModel(const QString& rootKey, const QString& keyField, const QString& parentField, QObject *parent= nullptr);
    virtual ~DataTreeModel();

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    QJsonObject getRecord(const QModelIndex& index);
protected:
    std::shared_ptr<DataTreeModelCommon> impl;
};


#endif //TROOPTREASURER_DATATREEMODEL_H
