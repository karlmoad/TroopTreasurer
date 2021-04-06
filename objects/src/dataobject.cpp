#include "objects/dataobject.h"

DataObjectImpl::DataObjectImpl()
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

DataObjectImpl::DataObjectImpl(const DataObjectImpl &other)
{
    this->_data = other._data;
}

DataObjectImpl::~DataObjectImpl(){}

QJsonValue DataObjectImpl::getValue(const QString &key)
{
    if(_data.contains(key))
    {
        return _data[key];
    }
    return QJsonValue();
}

void DataObjectImpl::setValue(const QString &key, const QJsonValue &value)
{
    _data[key]=value;
    if(key.compare("key",Qt::CaseSensitive) == 0)
    {
        _key = value.toString();
    }
}

QJsonObject DataObjectImpl::json() const
{
    return _data;
}

DataObjectImpl::DataObjectImpl(const QJsonObject &json)
{
    _data = json;
    if(_data.contains("key") && _key.compare(_data["key"].toString(), Qt::CaseSensitive) != 0)
    {
        _key = _data["key"].toString();
    }
}

QString DataObjectImpl::key() const
{
    return _key;
}

bool DataObjectImpl::isNull()
{
    return _data.isEmpty();
}

DataObjectImpl &DataObjectImpl::operator=(const DataObjectImpl &other)
{
    _key = other._key;
    _data = other._data;
    return *this;
}

bool DataObjectImpl::operator==(const DataObjectImpl &rhs) const
{
    return _key == rhs._key;
}

bool DataObjectImpl::operator!=(const DataObjectImpl &rhs) const
{
    return !(rhs == *this);
}
