#include "objects/importspecification.h"

class ImportSpecification::ImportSpecificationImpl
{
public:
    explicit ImportSpecificationImpl(const QString &name, const QString &target)
    {
        _spec.insert("name",name);
        _spec.insert("target",target);
    }

    explicit ImportSpecificationImpl(const QJsonObject& json)
    {
        _spec = json;
    }

    QList<QString> getMapFields()
    {
        if(_spec.contains("map"))
        {
            return _spec["map"].toObject().keys();
        }
        return QList<QString>();
    }

    QString getMapFieldExpression(const QString& name)
    {
        if(_spec.contains("map") && _spec["map"].toObject().contains(name))
        {
            return _spec["map"].toObject()[name].toString();
        }
        return QString();
    }

    void setMapFieldExpression(const QString& name, const QString& expression)
    {
        QJsonObject map = _spec["map"].toObject();

        if(map.contains(name))
        {
            map[name] = expression;
        }
        else
        {
            map.insert(name,expression);
        }

        _spec["map"] = map;
    }

    void remMapFieldExpression(const QString& name)
    {
        QJsonObject map = _spec["map"].toObject();
        if(map.contains(name))
        {
            map.remove(name);
        }
        _spec["map"] = map;
    }

    QJsonObject json() const
    {
        return _spec;
    }

    QString name() const
    {
        if(_spec.contains("name"))
        {
            return _spec["name"].toString();
        }
        return QString();
    }

    QString target() const
    {
        if(_spec.contains("target"))
        {
            return _spec["target"].toString();
        }
        return QString();
    }

private:
    QJsonObject _spec;
};

ImportSpecification::ImportSpecification(const QString &name, const QString &target)
{
    impl = new ImportSpecificationImpl(name, target);
}

ImportSpecification::ImportSpecification(const QJsonObject &json)
{
    impl = new ImportSpecificationImpl(json);
}

ImportSpecification::~ImportSpecification()
{
    delete impl;
}

QList<QString> ImportSpecification::getFields() const
{
    return impl->getMapFields();
}

QString ImportSpecification::getFieldExpression(const QString &id) const
{
    return impl->getMapFieldExpression(id);
}

void ImportSpecification::setField(const QString &id, const QString &expression)
{
    impl->setMapFieldExpression(id,expression);
}

void ImportSpecification::removeField(const QString &id)
{
    impl->remMapFieldExpression(id);
}

QJsonObject ImportSpecification::toJson() const
{
    return impl->json();
}

QString ImportSpecification::getName() const
{
    return impl->name();
}

QString ImportSpecification::getTarget() const
{
    return impl->target();
}


QList<ImportSpecification> ImportSpecificationFactory::Load(const QString &filename)
{
    QList<ImportSpecification> out;
    QFile file(filename);

    if(file.exists() && file.open(QIODevice::ReadOnly))
    {
        QJsonObject data = QJsonDocument::fromJson(file.readAll()).object();
        file.close();
        QJsonArray specs = data["templates"].toArray();
        for(int i = 0; i<specs.count(); i++)
        {
            out.append(ImportSpecification(specs[i].toObject()));
        }
    }
    else
    {
        ObjectError err("Unable to read master template file", static_cast<int>(ObjectErrorCode::ERROR_READ_FILE));
        err.raise();
    }

    return QList<ImportSpecification>();
}

void ImportSpecificationFactory::Save(const QList<ImportSpecification> &specs, const QString &filename)
{
    QJsonObject jdata;
    QJsonArray jspecs;

    for(int i =0; i<specs.count(); i++)
    {
        jspecs.append(specs[i].toJson());
    }

    jdata["templates"] = jspecs;


    QFile file(filename);
    if(file.open(QFile::ReadWrite | QFile::Text))
    {
        file.write(QJsonDocument(jdata).toJson());
        file.close();
    }
    else
    {
        ObjectError err("Unable to write master template file", static_cast<int>(ObjectErrorCode::ERROR_WRITE_FILE));
        err.raise();
    }
}
