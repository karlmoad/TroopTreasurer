#include <QFile>
#include "objects/schema.h"
#include "objects/objecterror.h"

class Schema::SchemaImpl
{
public:
    SchemaImpl(const QString& key, const QJsonObject& json): _name(key), _schemaDefinition(json)
    {}

    QString name() const
    {
        return _name;
    }

    QString display() const
    {
        if(_schemaDefinition.isEmpty() && _schemaDefinition.contains("display"))
        {
           return _schemaDefinition["display"].toString();
        }
        else
        {
            return QString();
        }
    }

    QString table() const
    {
        if(_schemaDefinition.isEmpty() && _schemaDefinition.contains("table"))
        {
            return _schemaDefinition["table"].toString();
        }
        else
        {
            return QString();
        }
    }

    bool importSupported() const
    {
        if(_schemaDefinition.isEmpty() && _schemaDefinition.contains("import"))
        {
            return _schemaDefinition["import"].toBool(false);
        }
        return false;
    }

    bool dupCheckSupported() const
    {
        if(_schemaDefinition.isEmpty() && _schemaDefinition.contains("dupcheck"))
        {
            return _schemaDefinition["dupcheck"].toBool(false);
        }
        return false;
    }

    bool truncateSupported() const
    {
        if(_schemaDefinition.isEmpty() && _schemaDefinition.contains("truncate"))
        {
            return _schemaDefinition["truncate"].toBool(false);
        }
        return false;
    }

    QMap <QString, QJsonObject> fields() const
    {
        QMap<QString,QJsonObject> fields;
        if(_schemaDefinition.isEmpty() && _schemaDefinition.contains("fields"))
        {
            QJsonArray flds = _schemaDefinition["fields"].toArray();
            for(int i =0; i<flds.count(); i++)
            {
                QJsonObject field = flds[i].toObject();
                fields.insert(field["name"].toString(), field);
            }
        }

        return fields;
    }

private:
    QJsonObject _schemaDefinition;
    QString _name;
};

Schema::Schema()
{

}

Schema::~Schema()
{}

QString Schema::getName() const
{
    return impl->name();
}

QString Schema::getDisplay() const
{
    return impl->display();
}

QString Schema::getTable() const
{
    return impl->table();
}

bool Schema::isImportSupported() const
{
    return impl->importSupported();
}

bool Schema::isDupCheckSupported() const
{
    return impl->dupCheckSupported();
}

bool Schema::isTruncateSupported() const
{
    return impl->truncateSupported();
}

QMap <QString, QJsonObject> Schema::getFields() const
{
    return impl->fields();
}

QMap <QString, Schema> Schema::load(const QString &filepath)
{
    QMap<QString,Schema> out;
    QFile file(filepath);
    if(file.open(QFile::ReadOnly | QFile::Text)){
        QByteArray raw = file.readAll();
        file.close();
        QJsonObject schemas = QJsonDocument::fromJson(raw).object();
        QList<QString> schemaKeys = schemas.keys();
        for(QString key: schemaKeys)
        {
            Schema s;
            s.impl = std::shared_ptr<SchemaImpl>(new SchemaImpl(key, schemas[key].toObject()));
            out.insert(key, s);
        }

    }
    else
    {
        ObjectError err("Unable to open and read schema definition file", static_cast<int>(ObjectErrorCode::ERROR_READ_FILE));
        err.raise();
    }
    return out;
}
