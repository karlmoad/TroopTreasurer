#include "objects/tablefield.h"

class TableField::TableFieldImpl: public InformationSchemaImpl
{
public:
    TableFieldImpl()=default;
    ~TableFieldImpl()=default();

    DataType dataType() const
    {
        return _type;
    }

    QString name() const
    {
        return _name;
    }

    QString format() const
    {
        return _format;
    }

    bool isKey() const
    {
        return _key;
    }

    bool isMatchKey() const
    {
        return _matchKey;
    }

    bool isNullable() const
    {
        return _nullable;
    }

    QVariant defaultValue() const
    {
        return _defaultVal;
    }

    int length() const
    {
        return _length;
    }

    virtual void initialize(const QJsonObject &json) override
    {
        if(json.contains("name") && json.contains("dt"))
        {
            _json = json;
            _type = DataType::NONE;
            _length = 0;
            _nullable = false;
            _key = false;
            _matchKey = false;
            _format = QString();
            _defaultVal = QVariant();

            _name = _json["name"].toString();

            QString dt = _json["dt"].toString();

            if(dt.trimmed().length() > 0)
            {
                if(dt.compare("string", Qt::CaseSensitive) == 0) _type=DataType::STRING;
                if(dt.compare("int", Qt::CaseSensitive) == 0) _type=DataType::INT;
                if(dt.compare("bool", Qt::CaseSensitive) == 0) _type=DataType::BOOLEAN;
                if(dt.compare("decimal", Qt::CaseSensitive) == 0) _type=DataType::DECIMAL;
                if(dt.compare("text", Qt::CaseSensitive) == 0) _type=DataType::TEXT;
            }

            if(_json.contains("length"))
                _length = _json["length"].toInt(0);

            if(_json.contains("format"))
                _format = _json["format"].toString();

            if(_json.contains("nullable"))
                _nullable = _json["nullable"].toBool(false);

            if(_json.contains("key"))
                _key= _json["key"].toBool(false);

            if(_json.contains("match_key"))
                _matchKey = _json["match_key"].toBool(false);

            if(_json.contains("default"))
                _defaultVal = _json["default"].toVariant();
        }
    }

private:
    QString _name;
    QString _format;
    QVariant _defaultVal;
    int _length;
    DataType _type;
    bool _nullable;
    bool _key;
    bool _matchKey;
};

TableField::TableField()
{

}

TableField::~TableField()
{

}

TableField::DataType TableField::getDataType() const
{
    return impl->dataType();
}

QString TableField::getName() const
{
    return impl->name();
}

QString TableField::getFormat() const
{
    return impl->format();
}

bool TableField::isKey() const
{
    return impl->isKey();
}

bool TableField::isMatchKey() const
{
    return impl->isMatchKey();
}

bool TableField::isNullable() const
{
    return impl->isNullable();
}

QVariant TableField::getDefaultValue() const
{
    return impl->defaultValue();
}

int TableField::getLength() const
{
    return impl->length();
}

QJsonObject TableField::toJson()
{
    return impl->json();
}

void TableField::initialize(const QJsonObject &json)
{
    if(json.contains("name") && json.contains("dt"))
    {
        impl = std::shared_ptr<TableFieldImpl>(new TableFieldImpl);
        impl->initialize(json);
    }
}

bool TableField::isNull() const
{
    return impl == nullptr || impl->isNull();
}