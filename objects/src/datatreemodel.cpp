#include "objects/datatreemodel.h"

DataTreeModel::DataTreeModel(const QString& rootKey, const QString& keyField, const QString& parentField, QObject *parent) :
                        QAbstractItemModel(parent),
                        impl(new DataTreeModelCommon(rootKey,keyField,parentField))
{

}

DataTreeModel::~DataTreeModel()
{

}

int DataTreeModel::rowCount(const QModelIndex &parent) const
{
    return impl->recordCount();
}

int DataTreeModel::columnCount(const QModelIndex &parent) const
{
    return impl->columnCount();
}

QVariant DataTreeModel::data(const QModelIndex &index, int role) const
{
    if( role != Qt::DisplayRole )
    {
        return QVariant();
    }

    if(index.column() < impl->columnCount())
    {
        QJsonObject header = impl->getHeader(index.column());
        QJsonObject record;

        HierarchyItem *item = static_cast<HierarchyItem*>(index.internalPointer());
        if(item)
        {
            record = impl->getRecord(*(item));
        }

        if(!record.isEmpty() && !header.isEmpty())
        {
            if(header.contains("field") && record.contains(header.value("field").toString()))
            {
                return record.value(header.value("field").toString()).toVariant();
            }
        }
    }
    return QVariant();
}

QVariant DataTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        QJsonObject header = impl->getHeader(section);
        return header.value("text").toString("");
    }
    return QVariant();
}

Qt::ItemFlags DataTreeModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex DataTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    HierarchyItem* parentAcct = impl->getHierarchyRef(parent);
    if (!parentAcct)
        return QModelIndex();

    HierarchyItem* subAcct = parentAcct->subItem(row);
    if (subAcct)
        return createIndex(row, column, subAcct);

    return QModelIndex();
}

QModelIndex DataTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    HierarchyItem *subAcct = impl->getHierarchyRef(child);
    HierarchyItem *parentAcct = subAcct ? subAcct->parent() : nullptr;

    if (impl->isRoot(parentAcct) || !parentAcct)
        return QModelIndex();

    return createIndex(parentAcct->subItemIndex(), 0, parentAcct);
}

QJsonObject DataTreeModel::getRecord(const QModelIndex &index)
{
    if(index.isValid())
    {
        HierarchyItem *item = static_cast<HierarchyItem*>(index.internalPointer());
        if(item)
        {
            return impl->getRecord(*(item));
        }
    }
    return QJsonObject();
}


