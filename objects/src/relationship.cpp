#include "objects/relationship.h"

//RELATIONSHIP
class Relationship::RelationshipImpl: public InformationSchemaImpl
{
public:
    RelationshipImpl(): _name(QString("UNKNOWN")),_type(RelationShipType::NONE), _table(QString("UNKNOWN")) {}
    virtual ~RelationshipImpl(){}

    QString name() const
    {
        return _name;
    }

    QString table() const
    {
        return _table;
    }

    RelationShipType type() const
    {
        return _type;
    }

    QList<std::shared_ptr<Reference>> fields() const
    {
        return _fields.values();
    }

    std::shared_ptr<Reference> field(const QString& name) const
    {
        if(_fields.contains(name))
        {
            return _fields[name];
        }
        return nullptr;
    }

    virtual void initialize(const QJsonObject &json) override
    {
        if(json.contains("method") && json["method"].toObject().contains("type") && json["method"].toObject().contains("table"))
        {
            _table = json["method"].toObject()["table"].toString();

            QString t = json["method"].toObject()["type"].toString();
            if(t.compare("table",Qt::CaseSensitive) == 0)
                _type = RelationShipType::TABLE;

            if(t.compare("table",Qt::CaseSensitive) == 0)
                _type = RelationShipType::DIRECT;

            if (json.contains("name"))
            {
                _name = json["name"].toString();
            }

            if(json.contains("fields"))
            {
                QJsonObject fields = json["fields"].toObject();
                for(QString key: fields.keys())
                {
                    QJsonObject obj = fields[key].toObject();
                    if(obj.contains("field"))
                    {
                        _fields.insert(key, std::shared_ptr<Reference>(new Reference(key, obj["field"].toString())));
                    }
                }
            }
        }
    }

private:
    QString _name;
    RelationShipType _type;
    QString _table;
    QMap<QString, std::shared_ptr<Reference>> _fields;
};

Relationship::Relationship(){}

Relationship::~Relationship(){}

Relationship::RelationShipType Relationship::getType() const
{
    if(impl)
        return impl->type();
    return Relationship::RelationShipType::NONE;
}

QString Relationship::getTableName() const
{
    if(impl)
        return impl->name();
    return QString("UNKNOWN");
}

QList<std::shared_ptr<Reference>> Relationship::getFields() const
{
    if(impl)
        return impl->fields();
    return QList<std::shared_ptr<Reference>>();
}

std::shared_ptr<Reference> Relationship::getField(const QString &name) const
{
    if(impl)
        return impl->field(name);
    return nullptr
}

QJsonObject Relationship::toJson()
{
    return QJsonObject();
}

bool Relationship::isNull() const
{
    return impl == nullptr || impl->isNull();
}

void Relationship::initialize(const QJsonObject &json)
{
    if(json.contains("method") && json["method"].toObject().contains("type") && json["method"].toObject().contains("table"))
    {
        impl = std::shared_ptr<RelationshipImpl>(new RelationshipImpl);
        impl->initialize(json);
    }
}

// RelationshipMapping

class RelationshipMapping::RelationshipMappingImpl : public InformationSchemaImpl
{
public:
    RelationshipMappingImpl(){}
    virtual ~RelationshipMappingImpl(){}

    QList<std::shared_ptr<Reference>> getMappingFull() const
    {
        return _map.values();
    }

    std::shared_ptr<Reference> getMapForField(const QString& field) const
    {
        if(_map.contains(field))
        {
            return _map[field];
        }
        return nullptr;
    }

    QString getRelationshipName() const
    {
        return _relName;
    }

    virtual void initialize(const QJsonObject &json) override
    {
        if(json.contains("relationship"))
        {
            _relName = json["relationship"].toString();
        }
        if(json.contains("map"))
        {
            QJsonArray mapArray = json["map"].toArray();
            for(int i = 0; i< mapArray.count(); i++)
            {
                QJsonObject mapItem = mapArray[i].toObject();
                if(mapItem.contains("field") && mapItem.contains("target"))
                {
                    auto item = std::shared_ptr<Reference>(new Reference(mapItem["field"].toString(), mapItem["target"].toString()));
                    _map.insert(mapItem["field"].toString(), item);
                }
            }
        }
    }

private:
    QMap<QString, std::shared_ptr<Reference>> _map;
    QString _relName;
};

RelationshipMapping::RelationshipMapping(){}

RelationshipMapping::~RelationshipMapping(){}

QString RelationshipMapping::getRelationshipName() const
{
    if(impl)
        return impl->getRelationshipName();
    else
        return QString();
}

QList<std::shared_ptr<Reference>> RelationshipMapping::getMapping() const
{
    if(impl)
        return impl->getMappingFull();
    else
        return QList<std::shared_ptr<ReferenceMap>>()
}

QJsonObject RelationshipMapping::toJson()
{
    return QJsonObject();
}

bool RelationshipMapping::isNull() const
{
    return impl == nullptr || impl->isNull();
}

void RelationshipMapping::initialize(const QJsonObject &json)
{
    if(json.contains("relationship") && json.contains("map"))
    {
        impl = std::shared_ptr<RelationshipMappingImpl>(new RelationshipMappingImpl);
        impl->initialize(json);
    }
}

std::shared_ptr<Reference> RelationshipMapping::getMapping(const QString &field) const
{
    if(impl)
        return impl->getMapForField(field);
    else
        return nullptr
}
