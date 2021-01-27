#include "objects/dataobject.h"

DataObject::DataObject()
{
    if(!_data.contains("key"))
    {
        _data["key"] = Utility::GenerateKey();
    }
    if(!_data.contains("date"))
    {
        _data["date"] = QDate::currentDate().toString(DateFormats::DATABASE_FORMAT);
    }
}

DataObject::DataObject(const DataObject &other)
{
    this->_data = other._data;
}

DataObject::~DataObject(){}

QJsonValue DataObject::getValue(const QString &key)
{
    if(_data.contains(key))
    {
        return _data[key];
    }
    return QJsonValue();
}

void DataObject::setValue(const QString &key, const QJsonValue &value)
{
    _data[key]=value;
}

const QJsonObject &DataObject::json()
{
    return _data;
}

DataObject::DataObject(const QJsonObject &json)
{
    _data = json;
}
