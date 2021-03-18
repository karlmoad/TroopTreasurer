#include "objects/relationship.h"


// REFERENCE MAP
ReferenceMap::ReferenceMap(const QString &origin, const QString &target): _origin(origin), _target(target){}

ReferenceMap::~ReferenceMap(){}

QString ReferenceMap::getOriginReference() const
{
    return _origin;
}

QString ReferenceMap::getTargetReference() const
{
    return _target;
}


//RELATIONSHIP
class Relationship::RelationshipImpl: public InformationSchemaImpl
{

};

Relationship::Relationship(){}

Relationship::~Relationship(){}

Relationship::RelationShipType Relationship::getType() const
{
    return Relationship::RelationShipType::NONE;
}

QMap<QString, std::shared_ptr<ReferenceMap>> Relationship::getReferenceMapForTable(const QString &table) const
{
    return nullptr;
}

QMap<QString, QMap<QString, std::shared_ptr<ReferenceMap>>> Relationship::getReferenceMaps() const
{
    return nullptr;
}

QString Relationship::getTableName() const
{
    return QString();
}

QJsonObject Relationship::toJson()
{
    return QJsonObject();
}

bool Relationship::isNull() const
{
    return false;
}

void Relationship::initialize(const QJsonObject &json)
{

}
