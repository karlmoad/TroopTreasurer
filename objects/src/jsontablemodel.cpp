#include <objects/jsontablemodel.h>

struct JsonTableModel::JsonTableModelImpl
{
    int _colCount;
    bool _errorState;
    QString _errorMsg;
    QMap<QString,QString> _headers;
    QVector<QJsonObject> _data;
};


JsonTableModel::JsonTableModel(QObject *parent) : QAbstractTableModel(parent), impl(new JsonTableModelImpl)
{

}

JsonTableModel::~JsonTableModel()
{}

int JsonTableModel::rowCount(const QModelIndex &parent) const
{
    return impl->_data.count();
}

QVariant JsonTableModel::data(const QModelIndex &index, int role) const
{
    if( role != Qt::DisplayRole )
    {
        return QVariant();
    }

    if(index.column()<impl->_headers.size())
    {
        QString column = impl->_headers.keys().at(index.column());
        QJsonObject data = impl->_data.at(index.row());

        if(data.contains(column))
        {
            return data[column].toVariant();
        }
        else
        {
            return QVariant();
        }
    }
    return QVariant();
}

QVariant JsonTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
                return impl->_headers.keys().at(section);
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

Qt::ItemFlags JsonTableModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
}

int JsonTableModel::columnCount(const QModelIndex &parent) const
{
    return impl->_headers.count();
}

void JsonTableModel::setHeaderMap(const QMap <QString, QString> &headers)
{
    impl->_headers = headers;
}

void JsonTableModel::setData(const QList <QJsonObject> &data)
{
    impl->_data = data.toVector();
}
