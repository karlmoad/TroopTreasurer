#include "objects/dataobject.h"

DataObject::DataObject()
{
    _key = Utility::GenerateKey();

    if(!_data.contains("key"))
    {
        _data["key"] = _key;
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
    if(key.compare("key",Qt::CaseSensitive) == 0)
    {
        _key = value.toString();
    }
}

const QJsonObject &DataObject::json()
{
    return _data;
}

DataObject::DataObject(const QJsonObject &json)
{
    _data = json;
    if(_data.contains("key") && _key.compare(_data["key"].toString(), Qt::CaseSensitive) != 0)
    {
        _key = _data["key"].toString();
    }
}

QString DataObject::key()
{
    return _key;
}

bool DataObject::isNull()
{
    return _data.isEmpty();
}
