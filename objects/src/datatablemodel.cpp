#include "objects/datatablemodel.h"

struct DataTableModel::DataTableModelImpl
{
    int _colCount;
    bool _errorState;
    QString _errorMsg;
    QVector<QString> _headers;
    QVector<QMap<QString, QVariant>> _data;
};

DataTableModel::DataTableModel(QObject *parent) : QAbstractTableModel(parent), impl(new DataTableModelImpl)
{}

DataTableModel::~DataTableModel()
{}

int DataTableModel::rowCount(const QModelIndex &parent) const
{
    return impl->_data.size();
}

QVariant DataTableModel::data(const QModelIndex &index, int role) const
{
    if( role != Qt::DisplayRole )
    {
        return QVariant();
    }

    if(index.column()<impl->_headers.size())
    {
        return impl->_data.at(index.row())[impl->_headers.at(index.column())];
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
            if(section < impl->_headers.size())
            {
                return impl->_headers.at(section);
            }
            return QVariant();
        }
        case Qt::Vertical:
        {
            return section + 1;
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
    return impl->_headers.size();
}

void DataTableModel::setHeaderMap(const QList<QString> &headers)
{
    this->impl->_headers = headers.toVector();
}

void DataTableModel::setData(const QList<QMap<QString, QVariant>> &data)
{
    beginResetModel();
    impl->_data = data.toVector();
    endResetModel();
}
