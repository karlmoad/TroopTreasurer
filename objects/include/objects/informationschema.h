#ifndef INFORMATIONSCHEMA_H
#define INFORMATIONSCHEMA_H

#include <memory>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QString>
#include <QList>
#include <QMap>

class Reference
{
public:
    Reference(const QString& name, const QString& target): _name(name), _target(target)
    {}

    virtual ~Reference()
    {}

    QString getName() const
    {
        return _name;
    }

    QString getTarget() const
    {
        return _target;
    }

private:
    QString _name;
    QString _target;
};

class InformationSchemaImpl
{
public:
    virtual void initialize(const QJsonObject&json)=0;
    bool isNull() const;
    QJsonObject json() const;
protected:
    QJsonObject _json;
};

bool InformationSchemaImpl::isNull() const
{
    return _json.isEmpty();
}

QJsonObject InformationSchemaImpl::json() const
{
    return _json;
}

class InformationSchema
{
public:
    virtual QJsonObject toJson()=0;
    virtual bool isNull() const =0;
    friend class InformationSchemaImpl;
    virtual void initialize(const QJsonObject &json)=0;
}

#endif //INFORMATIONSCHEMA_H