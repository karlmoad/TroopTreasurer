#include "objects/databaseschema.h"
#include <QJsonDocument>
#include <QFile>
#include "objects/settingsmanager.h"
#include "objects/objecterror.h"
#include "objects/tableschema.h"
#include "objects/relationship.h"

class DatabaseSchema::DatabaseSchemaImpl: public InformationSchemaImpl
{
public:
    DatabaseSchemaImpl()=default;
    ~DatabaseSchemaImpl()=default;

    std::shared_ptr<TableSchema> getTable(const QString& name) const
    {
        if(_tables.contains(name))
        {
            return _tables[name];
        }
        return nullptr;
    }

    QMap<QString, std::shared_ptr<TableSchema>> getTables() const
    {
        return _tables;
    }

    std::shared_ptr<Relationship> getRelationship(const QString & name) const
    {
        if(_relationships.contains(name))
        {
            return _relationships[name];
        }
        return nullptr;
    }

    QMap<QString, std::shared_ptr<Relationship>> getRelationships() const
    {
        return _relationships;
    }

    virtual void initialize(const QJsonObject &json) override
    {
        if(json.contains("tables"))
        {
            _json = json;
            QJsonObject tables = _json["tables"].toObject();
            for(QString key : tables.keys())
            {
                auto ts = std::shared_ptr<TableSchema>(new TableSchema);
                ts->initialize(tables[key].toObject());
                _tables.insert(key,ts);
            }
        }

        if(_json.contains("relationships"))
        {
            QJsonObject relationships = _json["relationships"].toObject();
            for(QString key : relationships.keys())
            {
                auto rel = std::shared_ptr<Relationship>(new Relationship);
                rel->initialize(relationships[key].toObject());
                _relationships.insert(key,rel);
            }
        }
    }
private:
    QMap<QString, std::shared_ptr<TableSchema>> _tables;
    QMap<QString, std::shared_ptr<Relationship>> _relationships;
};

DatabaseSchema::DatabaseSchema(){}

DatabaseSchema::~DatabaseSchema(){}

std::shared_ptr<TableSchema> DatabaseSchema::getTable(const QString &name) const
{
    return impl->getTable(name);
}

QMap<QString, std::shared_ptr<TableSchema>> DatabaseSchema::getTables() const
{
    return impl->getTables();
}

QJsonObject DatabaseSchema::toJson()
{
    return impl->json();
}

void DatabaseSchema::initialize(const QJsonObject &json)
{
    if(json.contains("tables"))
    {
        impl = std::shared_ptr<DatabaseSchemaImpl>(new DatabaseSchemaImpl);
        impl->initialize(json);
    }
}

std::shared_ptr<DatabaseSchema> DatabaseSchema::retrieve()
{
    //only load if no other instance exists
    if(_instance == nullptr)
    {
        SettingsManager *sm = SettingsManager::getInstance();
        QJsonObject meta = sm->getConfigurationSectionMetadata("database");

        if (!meta.contains("schema"))
            return nullptr;

        QFile file(meta["schema"].toString());
        if (!file.open(QFile::ReadOnly | QFile::Text))
        {
            return nullptr;
        }

        QByteArray raw = file.readAll();
        file.close();
        QJsonObject schema = QJsonDocument::fromJson(raw).object();

        auto dbs = std::shared_ptr<DatabaseSchema>(new DatabaseSchema);
        dbs->initialize(schema);
        _instance = dbs;
    }

    return _instance;
}

bool DatabaseSchema::isNull() const
{
    return impl == nullptr || impl->isNull();
}

std::shared_ptr<Relationship> DatabaseSchema::getRelationship(const QString &name) const
{
    return impl->getRelationship(name);
}

QMap<QString, std::shared_ptr<Relationship>> DatabaseSchema::getRelationships() const
{
    return impl->getRelationships();
}
