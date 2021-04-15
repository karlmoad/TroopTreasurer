#include "objects/tableschema.h"
#include "objects/relationship.h"
#include "objects/tablefield.h"

class TableSchema::TableSchemaImpl : public InformationSchemaImpl
{
public:
    TableSchemaImpl()=default;
    ~TableSchemaImpl()=default;

    bool isImportSupported() const
    {
        return _json.contains("operations") && _json["operations"].toObject().contains("import");
    }

    ImportOptions importOptions() const
    {
        ImportOptions opts;
        if(isImportSupported())
        {
            QJsonObject importOpts = _json["operations"].toObject()["import"].toObject();

            if(importOpts.contains("truncate") && importOpts["truncate"].toBool())
                opts.setTruncateEnabled(true);
            else
                opts.setTruncateEnabled(false);

            if(importOpts.contains("update") && importOpts["update"].toBool())
                opts.setUpdateEnabled(true);
            else
                opts.setUpdateEnabled(false);

            if(importOpts.contains("dupcheck") && importOpts["dupcheck"].toBool())
                opts.setDupCheckEnabled(true);
            else
                opts.setDupCheckEnabled(false);

            if(importOpts.contains("append") && importOpts["append"].toBool())
                opts.setAppendEnabled(true);
            else
                opts.setAppendEnabled(false);

            if(importOpts.contains("archive") && importOpts["archive"].toBool())
                opts.setArchiveEnabled(true);
            else
                opts.setArchiveEnabled(false);
        }
        return opts;
    }

    QString tableName() const
    {
        if(_json.contains("table"))
        {
            return _json["table"].toString();
        }
        return QString();
    }

    QString displayName() const
    {
        if(_json.contains("display"))
        {
            return _json["display"].toString();
        }
        return QString();
    }

    bool isArchiveSupported() const
    {
        return _json.contains("operations") && _json["operations"].toObject().contains("archive");
    }

    QString archiveTable() const
    {
        if(isArchiveSupported() && _json["operations"].toObject()["archive"].toObject().contains("table"))
        {
            return _json["operations"].toObject()["archive"].toObject()["table"].toString();
        }
        return QString();
    }

    bool isBindingSupported() const
    {
        return _json.contains("operations") && _json["operations"].toObject().contains("binding");
    }

    QList<Reference> binding() const
    {
        QList<Reference> out;
        if(isBindingSupported() && _json["operations"].toObject()["binding"].toObject().contains("map"))
        {
            QJsonArray mappings = _json["operations"].toObject()["binding"].toObject()["map"].toArray();
            for(int i = 0; i< mappings.count(); i++)
            {
                QJsonObject mapping = mappings.at(i).toObject();
                out.append(Reference(mapping["property"].toString(), mapping["field"].toString()));
            }
        }
        return out;
    }

    std::shared_ptr<TableField> getField(const QString& ref) const
    {
        if(_fields.contains(ref))
            return _fields[ref];
        return nullptr;
    }

    QMap<QString,std::shared_ptr<TableField>> getFields() const
    {
        return _fields;
    }

    std::shared_ptr<RelationshipMapping> getRelationshipReference(const QString& name) const
    {
        if(_relationships.contains(name))
            return _relationships[name];
        return nullptr;
    }

    QMap<QString,std::shared_ptr<RelationshipMapping>> getRelationshipReferences() const
    {
        return _relationships;
    }

    virtual void initialize(const QJsonObject &json) override
    {
        if(json.contains("fields"))
        {
            _json = json;
            QJsonObject tables = _json["fields"].toObject();
            for(QString key : tables.keys())
            {
                auto tf = std::shared_ptr<TableField>(new TableField);
                tf->initialize(tables[key].toObject());
                _fields.insert(key,tf);
            }
        }

        if(_json.contains("relationships"))
        {
            QJsonObject relationships = _json["relationships"].toObject();
            for(QString key : relationships.keys())
            {
                auto rel = std::shared_ptr<RelationshipMapping>(new RelationshipMapping);
                rel->initialize(relationships[key].toObject());
                _relationships.insert(key,rel);
            }
        }
    }

private:
    QMap<QString,std::shared_ptr<TableField>> _fields;
    QMap<QString,std::shared_ptr<RelationshipMapping>> _relationships;
};


TableSchema::TableSchema()
{}

TableSchema::~TableSchema()
{}

std::shared_ptr<TableField> TableSchema::getField(const QString &ref) const
{
    return impl->getField(ref);
}

QMap<QString, std::shared_ptr<TableField>> TableSchema::getFields() const
{
    return impl->getFields();
}

QJsonObject TableSchema::toJson()
{
    return QJsonObject();
}

void TableSchema::initialize(const QJsonObject &json)
{
    if(json.contains("fields") && json.contains("table"))
    {
        impl = std::shared_ptr<TableSchemaImpl>(new TableSchemaImpl);
        impl->initialize(json);
    }
}

bool TableSchema::isNull() const
{
    return impl == nullptr || impl->isNull();
}

std::shared_ptr<RelationshipMapping> TableSchema::getRelationshipReference(const QString &name) const
{
    return impl->getRelationshipReference(name);
}

QMap<QString, std::shared_ptr<RelationshipMapping>> TableSchema::getRelationshipReferences() const
{
    return impl->getRelationshipReferences();
}

bool TableSchema::isImportSupported() const
{
    return impl->isImportSupported();
}

ImportOptions TableSchema::importOptions() const
{
    return impl->importOptions();
}

bool TableSchema::isArchiveSupported() const
{
    return impl->isArchiveSupported();
}

QString TableSchema::archiveTable() const
{
    return impl->archiveTable();
}

QString TableSchema::tableName() const
{
    return impl->tableName();
}

QString TableSchema::displayName() const
{
    return impl->displayName();
}

bool TableSchema::isBindingSupported() const
{
    return impl->isBindingSupported();
}

const QList<Reference>& TableSchema::getMapping() const
{
    return QList<Reference>();
}
