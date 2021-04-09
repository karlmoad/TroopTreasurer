#include "objects/databasecontroller.h"

class DatabaseController::DatabaseControllerImpl
{
private:
    QMap<QString,std::shared_ptr<Relationship>> _relationships;
    QMap<QString, QString> _relationshipValues;
    QMap<QString, QVariant> _args;
    std::shared_ptr<TableSchema> _table;
};

DatabaseController::DatabaseController(std::shared_ptr<TableSchema> tableSchema)
{

}

DatabaseController::~DatabaseController()
{

}

void DatabaseController::addRelationship(std::shared_ptr<Relationship> relationship, const QString &fieldName,
                                         const QString &fieldValue)
{

}

void DatabaseController::addMapping(const QString &jsonKey, const QString &fieldBinding)
{

}

void DatabaseController::setArguments(const QMap<QString, QVariant> &args)
{

}

ResultStatus DatabaseController::load(QList<QJsonObject> &results)
{
    return ResultStatus();
}

ResultStatus DatabaseController::add(const QJsonObject &objectJson)
{
    return ResultStatus();
}

ResultStatus DatabaseController::update(const QJsonObject &jsonObject)
{
    return ResultStatus();
}

ResultStatus DatabaseController::remove(const QJsonObject &jsonObject)
{
    return ResultStatus();
}





