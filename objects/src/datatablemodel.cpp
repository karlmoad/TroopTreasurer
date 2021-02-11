#include "objects/datatablemodel.h"
#include "objects/objecterror.h"
#include <QVector>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

DataTableModel::DataTableModel(QObject *parent) : QAbstractTableModel(parent), impl(new DataTableModelImpl)
{}

DataTableModel::~DataTableModel()
{}

int DataTableModel::rowCount(const QModelIndex &parent) const
{
    return impl->count();
}

QVariant DataTableModel::data(const QModelIndex &index, int role) const
{
    if( role != Qt::DisplayRole )
    {
        return QVariant();
    }

    if(index.column() < impl->columnCount())
    {
        std::shared_ptr<QJsonObject> header = impl->getHeader(index.column());
        std::shared_ptr<QJsonObject> record = impl->getRecord(index.row());

        if(record && header)
        {
            if(header->contains("field") && record->contains(header->value("field").toString()))
            {
                return record->value(header->value("field").toString()).toVariant();
            }
        }
    }
    return QVariant();
}

QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( role != Qt::DisplayRole )
    {
        return QVariant();
    }

    switch(orientation)
    {
        case Qt::Horizontal:
        {
            if(impl->isHorizontalColumnsEnabled())
            {
                std::shared_ptr<QJsonObject> header= impl->getHeader(section);
                if(header)
                {
                    return header->value("header").toString();
                }
            }
            return QVariant();
        }
        case Qt::Vertical:
        {
            if(impl->isVerticalColumnsEnabled())
            {
                return section + 1;
            }
            return QVariant();
        }
        default:
            return QVariant();
    }
}

Qt::ItemFlags DataTableModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
}

int DataTableModel::columnCount(const QModelIndex &parent) const
{
    return impl->columnCount();
}
