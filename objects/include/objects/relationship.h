#ifndef RELATIONSHIP_H
#define RELATIONSHIP_H

#include "informationschema.h"

class ReferenceMap
{
public:
    ReferenceMap(const QString& origin, const QString& target);
    virtual ~ReferenceMap();

    QString getOriginReference() const;
    QString getTargetReference() const;

private:
    QString _origin;
    QString _target;
};

class Relationship: public InformationSchema
{
public:
    enum class RelationShipType {NONE=0, TABLE=1, DIRECT=2};

    Relationship();
    virtual ~Relationship();

    RelationShipType getType() const;
    QMap<QString, std::shared_ptr<ReferenceMap>> getReferenceMapForTable(const QString &table) const;
    QMap<QString, QMap<QString, std::shared_ptr<ReferenceMap>>> getReferenceMaps() const;
    QString getTableName() const;

    virtual QJsonObject toJson() override;
    virtual bool isNull() const override;

private:
    friend class DatabaseSchema;
    virtual void initialize(const QJsonObject &json) override;


private:
    class RelationshipImpl;
    std::shared_ptr<RelationshipImpl> impl;
};

class RelationshipReference: public InformationSchema
{
public:
    RelationshipReference();
    virtual ~RelationshipReference();

    std::shared_ptr<Relationship> getRelationship() const;
    QList<std::shared_ptr<ReferenceMap>> getMapping() const;

    virtual QJsonObject toJson() override;
    virtual bool isNull() const override;

private:
    friend class DatabaseSchema;
    virtual void initialize(const QJsonObject &json) override;

private:
    class RelationshipReferenceImpl;
    std::shared_ptr<RelationshipReferenceImpl> impl;
};

#endif //RELATIONSHIP_H
