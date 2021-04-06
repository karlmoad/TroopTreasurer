#ifndef TROOPTREASURER_DATAACCESSOBJECT_H
#define TROOPTREASURER_DATAACCESSOBJECT_H

#include <functional>
#include <QModelIndex>
#include <QObject>
#include <QString>
#include <QList>
#include <QJsonObject>
#include <memory>
#include "resultstatus.h"
#include "tableschema.h"

template <typename T>
class DataAccessObject
{
public:
    DataAccessObject() : _itemCount(nullptr),
                        _indexOfKey(nullptr),
                        _getItem(nullptr),
                        _getJson(nullptr),
                        _addItem(nullptr),
                        _updateItem(nullptr),
                        _removeItem(nullptr)
    {}

    ~DataAccessObject()
    {}

    int count()
    {
        if(_itemCount)
            return _itemCount();
        else
            return 0;
    }

    int indexOf(const QString& key)
    {
        if(_indexOfKey)
            return _indexOfKey(key);
        else
            return -1;
    }

    T getObject(const QString& key)
    {
        return getObject(indexOf(key));
    }

    T getObject(int index)
    {
        if(_getItem)
        {
            if(index >= 0)
                return _getItem(index);
            else
                return T();
        }
        else
            return T();
    }

    QJsonObject getJson(const QString& key)
    {
        return getJson(indexOf(key));
    }

    QJsonObject getJson(int index)
    {
        if(_getJson)
        {
            if(index >= 0)
                return _getJson(index);
            else
                return QJsonObject();
        }
        else
            return QJsonObject();
    }

    ResultStatus addObject(const T& object)
    {
        if(_addItem)
            return _addItem(object);
        else
            return ResultStatus::Builder().setStatus(ResultStatus::Status::ERROR).setMessage("No Binding Available").build();
    }

    ResultStatus updateObject(const T& object)
    {
        if(_updateItem)
            return _updateItem(object);
        else
            return ResultStatus::Builder().setStatus(ResultStatus::Status::ERROR).setMessage("No Binding Available").build();
    }

    ResultStatus removeObject(int index)
    {
        if(_removeItem)
            return _removeItem(index);
        else
            return ResultStatus::Builder().setStatus(ResultStatus::Status::ERROR).setMessage("No Binding Available").build();
    }

    void bindContextItemCount(std::function<int()> itemCountFunc)
    {
        _itemCount = itemCountFunc;
    }

    void bindContextIndexForKey(std::function<int(const QString&)> indexKeyFunc)
    {
        _indexOfKey = indexKeyFunc;
    }

    void bindContextGet(std::function<const T& (int)> getItemFunc)
    {
        _getItem = getItemFunc;
    }

    void bindContextJson(std::function<QJsonObject(int)> getJsonFunc)
    {
        _getJson = getJsonFunc;
    }

    void bindContextAdd(std::function<ResultStatus(const T&)> addItemFunc)
    {
        _addItem = addItemFunc;
    }

    void bindContextUpdate(std::function<ResultStatus(const T&)> updateItemFunc)
    {
        _updateItem = updateItemFunc;
    }

    void bindContextRemove(std::function<ResultStatus(int)> removeItemFunc)
    {
        _removeItem = removeItemFunc;
    }

private:
    std::function<int()> _itemCount;
    std::function<int(const QString&)> _indexOfKey;
    std::function<const T& (int)> _getItem;
    std::function<QJsonObject(int)> _getJson;
    std::function<ResultStatus(const T&)> _addItem;
    std::function<ResultStatus(const T&)> _updateItem;
    std::function<ResultStatus(int)> _removeItem;
};
#endif //TROOPTREASURER_DATAACCESSOBJECT_H
