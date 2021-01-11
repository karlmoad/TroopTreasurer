#include "objects/importspecification.h"

class ImportSpecification::ImportSpecificationImpl
{
public:
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
        QJsonObject map;

        if(_spec.contains("map"))
        {
            map = _spec["map"].toObject();
        }

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

    void clearFields()
    {
        _spec["map"] = QJsonObject();
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

ImportSpecification::ImportSpecification(const QJsonObject &json): impl(new ImportSpecificationImpl(json))
{}

ImportSpecification::~ImportSpecification(){}

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

ImportSpecification ImportSpecification::initializeNew(const QString &name, const Schema &schema)
{
    QJsonObject json;
    json["name"] = name;
    json["target"] = schema.getName();

    ImportSpecification spec(json);

    //Initialize fields to empty base don schema available fields
    for(QString field :schema.getFields().keys())
    {
        spec.setField(field,QString());
    }

    return spec;
}

void ImportSpecification::clearFields()
{
    impl->clearFields();
}

QList<ImportSpecification> ImportSpecificationController::Load(const QString &filename)
{
    QList<ImportSpecification> out;
    QFile file(filename);

    if(file.exists())
    {
        if (file.open(QIODevice::ReadOnly))
        {
            QJsonObject data = QJsonDocument::fromJson(file.readAll()).object();
            file.close();
            QJsonArray specs = data["templates"].toArray();
            for (int i = 0; i < specs.count(); i++)
            {
                out.append(ImportSpecification(specs[i].toObject()));
            }
        } else
        {
            ObjectError err("Unable to read master template file", static_cast<int>(ObjectErrorCode::ERROR_READ_FILE));
            err.raise();
        }
    }
    else
    {
        ObjectError err("No master template file found", static_cast<int>(ObjectErrorCode::ERROR_NO_FILE));
        err.raise();
    }

    return out;
}

void ImportSpecificationController::Save(const ImportSpecification &spec, const QString &filename)
{
    QJsonObject json = ImportSpecificationController::open(filename);

    //find if spec currently exists or not, add/amend
    if(json.isEmpty()) return;

    bool bFound = false;
    int fIndex = -1;
    QJsonArray specs = json["templates"].toArray();
    for (int i = 0; i < specs.count(); i++)
    {
        QJsonObject temp = specs[i].toObject();
        if(!temp.isEmpty() && temp.contains("name") && spec.getName().compare(temp["name"].toString(), Qt::CaseSensitive)==0)
        {
            bFound = true;
            fIndex = i;
            break;
        }
    }

    if(bFound && fIndex > -1)
    {
        specs[fIndex] = spec.toJson();
    }
    else
    {
        specs.append(spec.toJson());
    }

    // save definition structure back to disk
    json["templates"] = specs;

    ImportSpecificationController::write(json, filename);
}

void ImportSpecificationController::Delete(const ImportSpecification &spec, const QString &filename)
{
    QJsonObject json = ImportSpecificationController::open(filename);

    //find if spec currently exists or not, add/amend
    if(json.isEmpty()) return;

    bool bFound = false;
    int fIndex = -1;
    QJsonArray specs = json["templates"].toArray();
    for (int i = 0; i < specs.count(); i++)
    {
        QJsonObject temp = specs[i].toObject();
        if(!temp.isEmpty() && temp.contains("name") && spec.getName().compare(temp["name"].toString(), Qt::CaseSensitive)==0)
        {
            bFound = true;
            fIndex = i;
            break;
        }
    }

    if(bFound && fIndex > -1)
    {
        specs.removeAt(fIndex);
    }

    // save definition structure back to disk
    json["templates"] = specs;

    ImportSpecificationController::write(json, filename);
}

QJsonObject ImportSpecificationController::open(const QString &filename)
{
    //get the current templates definition file contents into json object
    QFile file(filename);
    QJsonObject out;
    if(file.exists())
    {
        if (file.open(QIODevice::ReadOnly))
        {
            out = QJsonDocument::fromJson(file.readAll()).object();
            file.close();
        } else
        {
            ObjectError err("Unable to read master template file", static_cast<int>(ObjectErrorCode::ERROR_READ_FILE));
            err.raise();
        }
    }
    else
    {
        ObjectError err("No master template file found", static_cast<int>(ObjectErrorCode::ERROR_NO_FILE));
        err.raise();
    }
    return out;
}

void ImportSpecificationController::write(const QJsonObject &json, const QString &filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
    {
        file.write(QJsonDocument(json).toJson());
        file.close();
    }
    else
    {
        ObjectError err("Unable to write master template file", static_cast<int>(ObjectErrorCode::ERROR_WRITE_FILE));
        err.raise();
    }
}
