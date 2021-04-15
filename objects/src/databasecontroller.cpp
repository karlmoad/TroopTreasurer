#include "objects/databasecontroller.h"

class DatabaseController::DatabaseControllerImpl
{
public:
    DatabaseControllerImpl(std::shared_ptr<TableSchema> table, DataResponsibility responsibility): _table(table), _responsibility(responsibility){}
    ~DatabaseControllerImpl()=default;

    void setArguments(const QMap<QString, QVariant> &args)
    {
        _args = args;
    }

    void addRelationship(std::shared_ptr<Relationship> relationship, const QString &fieldName, const QString &fieldValue)
    {
        _relationships[fieldName] = relationship;
        _relationshipValues[fieldName] = fieldValue;
    }

    void addMapping(const QString &jsonKey, const QString &fieldBinding)
    {
        _field2ObjectMap.insert(fieldBinding, jsonKey);
        _object2FieldMap.insert(jsonKey, fieldBinding);
    }

    ResultStatus load(QList<QJsonObject> & results)
    {
        return ResultStatus();
    }

    ResultStatus add(const QJsonObject &json)
    {
        return ResultStatus();
    }

    ResultStatus update(const QJsonObject &json)
    {
        return ResultStatus();
    }

    ResultStatus remove(const QJsonObject &json)
    {
        return ResultStatus();
    }

private:


    QString renderFromStatement()
    {
        QString statement = QString("FROM %1").arg(_table->tableName());


    }



    QMap<QString,std::shared_ptr<Relationship>> _relationships;
    QMap<QString, QString> _relationshipValues;
    QMap<QString, QVariant> _args;
    QMap<QString, QString> _object2FieldMap;
    QMap<QString, QString> _field2ObjectMap;
    std::shared_ptr<TableSchema> _table;
    DataResponsibility _responsibility;
};

DatabaseController::DatabaseController(std::shared_ptr<TableSchema> tableSchema, DataResponsibility responsibility): impl(new DatabaseControllerImpl(tableSchema, responsibility))
{}

DatabaseController::~DatabaseController()
{}

void DatabaseController::addRelationship(std::shared_ptr<Relationship> relationship, const QString &fieldName,
                                         const QString &fieldValue)
{
    impl->addRelationship(relationship, fieldName, fieldValue);
}

void DatabaseController::addMapping(const QString &jsonKey, const QString &fieldBinding)
{
    impl->addMapping(jsonKey, fieldBinding);
}

void DatabaseController::setArguments(const QMap<QString, QVariant> &args)
{
    impl->setArguments(args);
}

ResultStatus DatabaseController::load(QList<QJsonObject> &results)
{
    return impl->load(results);
}

ResultStatus DatabaseController::add(const QJsonObject &jsonObject)
{
    return impl->add(jsonObject);
}

ResultStatus DatabaseController::update(const QJsonObject &jsonObject)
{
    return impl->update(jsonObject);
}

ResultStatus DatabaseController::remove(const QJsonObject &jsonObject)
{
    return impl->remove(jsonObject);
}





