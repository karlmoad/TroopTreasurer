#ifndef RELATIONSHIP_H
#define RELATIONSHIP_H

#include "informationschema.h"

class Relationship: public InformationSchema
{
public:
    enum class RelationShipType {NONE=0, TABLE=1, DIRECT=2};

    Relationship();
    virtual ~Relationship();

    RelationShipType getType() const;
    QString getTableName() const;
    QList<std::shared_ptr<Reference>> getFields() const;
    std::shared_ptr<Reference> getField(const QString& name) const;


    virtual QJsonObject toJson() override;
    virtual bool isNull() const override;
    virtual void initialize(const QJsonObject &json) override;

private:
    class RelationshipImpl;
    std::shared_ptr<RelationshipImpl> impl;
};

class RelationshipMapping: public InformationSchema
{
public:
    RelationshipMapping();
    virtual ~RelationshipMapping();

    QString getRelationshipName() const;
    QList<std::shared_ptr<Reference>> getMapping() const;
    std::shared_ptr<Reference> getMapping(const QString& field) const;

    virtual QJsonObject toJson() override;
    virtual bool isNull() const override;
    virtual void initialize(const QJsonObject &json) override;

private:
    class RelationshipMappingImpl;
    std::shared_ptr<RelationshipMappingImpl> impl;
};

#endif //RELATIONSHIP_H
