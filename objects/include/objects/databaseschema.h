#ifndef DATABASESCHEMA_H
#define DATABASESCHEMA_H

#include "informationschema.h"
#include "relationship.h"


class TableSchema;
class Relationship;

class DatabaseSchema: public InformationSchema
{
public:
    DatabaseSchema();
    virtual ~DatabaseSchema();

    std::shared_ptr<TableSchema> getTable(const QString& name) const;
    QMap<QString, std::shared_ptr<TableSchema>> getTables() const;
    std::shared_ptr<Relationship> getRelationship(const QString& name) const;
    QMap<QString, std::shared_ptr<Relationship>> getRelationships() const;
    virtual bool isNull() const override;
    virtual QJsonObject toJson() override;
    virtual void initialize(const QJsonObject &json) override;

    static std::shared_ptr<DatabaseSchema> retrieve();

private:
    class DatabaseSchemaImpl;
    std::shared_ptr<DatabaseSchemaImpl> impl;

    static std::shared_ptr<DatabaseSchema> _instance;
};

#endif //DATABASESCHEMA_H
