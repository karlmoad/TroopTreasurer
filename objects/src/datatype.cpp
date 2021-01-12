#include "objects/datatype.h"

DataType::DataType()
{
    this->_type = DataTypes::Type::UNDEFINED;
}

DataType::DataType(DataTypes::Type type)
{
    this->_type = type;
}

DataType::DataType(const QString &type): DataType()
{
    this->setType(type);
}

DataType::~DataType()
{}

DataTypes::Type DataType::type() const
{
    return this->_type;
}

bool DataType::isQuoted() const
{
    return DataTypes::IsQuotedType(this->_type);
}

QString DataType::toString() const
{
    return DataTypes::TypeToString(this->_type);
}

void DataType::setType(DataTypes::Type type)
{
    this->_type = type;
}

void DataType::setType(const QString &typeString)
{
    this->_type = DataTypes::StringToType(typeString);
}

