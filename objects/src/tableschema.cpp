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
        return _json.contains("options") && _json["options"].toObject().contains("import");
    }

    ImportOptions importOptions() const
    {
        ImportOptions opts;
        if(isImportSupported())
        {
            QJsonObject importOpts = _json["options"].toObject()["import"].toObject();

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

    bool isArchiveSupported() const
    {
        return _json.contains("options") && _json["options"].toObject().contains("archive");
    }

    QString archiveTable() const
    {
        if(isArchiveSupported() && _json["options"].toObject()["archive"].toObject().contains("table"))
        {
            return _json["options"].toObject()["archive"].toObject()["table"].toString();
        }
        return QString();
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

    std::shared_ptr<RelationshipReference> getRelationshipReference(const QString& name) const
    {
        if(_relationships.contains(name))
            return _relationships[name];
        return nullptr;
    }

    QMap<QString,std::shared_ptr<RelationshipReference>> getRelationshipReferences() const
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
                auto rel = std::shared_ptr<RelationshipReference>(new RelationshipReference);
                rel->initialize(relationships[key].toObject());
                _relationships.insert(key,rel);
            }
        }
    }

private:
    QMap<QString,std::shared_ptr<TableField>> _fields;
    QMap<QString,std::shared_ptr<RelationshipReference>> _relationships;
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

std::shared_ptr<RelationshipReference> TableSchema::getRelationshipReference(const QString &name) const
{
    return impl->getRelationshipReference(name);
}

QMap<QString, std::shared_ptr<RelationshipReference>> TableSchema::getRelationshipReferences() const
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
