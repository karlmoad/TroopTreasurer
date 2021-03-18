#ifndef TROOPTREASURER_DATAOBJECTMODEL_H
#define TROOPTREASURER_DATAOBJECTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonValue>
#include "objects/datatablemodel.h"
#include "objects/dao.h"
#include "resultstatus.h"
#include "dataobject.h"
#include <memory>
#include <type_traits>

template <typename T>
class DataObjectModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DataObjectModel(std::shared_ptr<DataAccessObject<T>> dao = nullptr, QObject *parent= nullptr);
    virtual ~DataObjectModel();
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual T getObject(const QModelIndex &index);
    virtual ResultStatus addObject(const T &object);
    virtual ResultStatus updateObject(const QModelIndex& index, const T &object);
    virtual ResultStatus removeObject(const QModelIndex& Index);
    virtual void addHeader(const QString& field, const QString& header);
    virtual ResultStatus load(const QMap<QString,QString> &params);
    virtual void load(const QList<T>& items);

protected:
    std::shared_ptr<DataAccessObject<T>> _dao;
    QList<QJsonObject> _headers

private:
    static_assert(std::is_base_of<DataObjectInterface, T>::value, "T is not derived from DataObjectInterface");
};

template<typename T>
DataObjectModel<T>::DataObjectModel(std::shared_ptr<DataAccessObject<T>> dao, QObject *parent): QAbstractTableModel(parent), _dao(dao)
{}

template<typename T>
DataObjectModel<T>::~DataObjectModel()
{}

template<typename T>
int DataObjectModel<T>::rowCount(const QModelIndex &parent) const
{
    return _dao->count();
}

template<typename T>
QVariant DataObjectModel<T>::data(const QModelIndex &index, int role) const
{
    if( role != Qt::DisplayRole )
    {
        return QVariant();
    }

    if(index.column() < _headers.count())
    {
        QJsonObject obj = _dao->getJson(index.column());
        if(!obj.isEmpty())
        {
            QJsonObject header = _headers[index.column()];
            if (header.contains("field") && obj.contains(header.value("field").toString()))
            {
                return obj.value(header.value("field").toString()).toVariant();
            }
        }
    }
    return QVariant();
}

template<typename T>
QVariant DataObjectModel<T>::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( role != Qt::DisplayRole )
    {
        return QVariant();
    }

    switch(orientation)
    {
        case Qt::Horizontal:
        {
            if(section < _headers.count())
            {
                QJsonObject header= _headers[section];
                if(header)
                {
                    return header.value("header").toString();
                }
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

template<typename T>
Qt::ItemFlags DataObjectModel<T>::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
}

template<typename T>
int DataObjectModel<T>::columnCount(const QModelIndex &parent) const
{
    return _headers.count();
}

template<typename T>
T DataObjectModel<T>::getObject(const QModelIndex &index)
{
    return _dao->getObject(index.row);
}

template<typename T>
ResultStatus DataObjectModel<T>::addObject(const T &object)
{
    ResultStatus r = _dao->addObject(object);
    if(!r.isError() && r.status())
    {
        int idx = rowCount(QModelIndex());
        beginInsertRows(QModelIndex(),idx,idx);
        endInsertRows();
    }
    return r;
}

template<typename T>
ResultStatus DataObjectModel<T>::updateObject(const QModelIndex &index, const T &object)
{
    ResultStatus r = _dao->updateObject(object);
    if(!r.isError() && r.status())
    {
        emit dataChanged(index, index);
    }
    return r;
}

template<typename T>
ResultStatus DataObjectModel<T>::removeObject(const QModelIndex &index)
{
    ResultStatus r = _dao->removeObject(index.row())
    if(!r.isError() && r.status())
    {
        beginRemoveRows(QModelIndex(), index.row(), index.row());
        endRemoveRows();
    }
    return r;
}

template<typename T>
void DataObjectModel<T>::addHeader(const QString &field, const QString &header)
{
    beginInsertColumns(QModelIndex(), _headers.count(), _headers.count());
    QJsonObject header = QJsonObject();
    header["field"] = field;
    header["header"] = header;
    _headers.append(header);
    endInsertColumns();
}

template<typename T>
ResultStatus DataObjectModel<T>::load(const QMap<QString, QString> &params)
{
    return _dao->loadObjects(params);
}

template<typename T>
void DataObjectModel<T>::load(const QList<T> &items)
{
    _dao->loadObjects(items);
}

#endif //TROOPTREASURER_DATAOBJECTMODEL_H
