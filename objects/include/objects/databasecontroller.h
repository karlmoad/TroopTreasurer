#ifndef DATABASEOPERATION_H
#define DATABASEOPERATION_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QJsonObject>
#include <QList>
#include <memory>
#include "resultstatus.h"
#include "relationship.h"
#include "tableschema.h"
#include "objects.h"
#include "dataresponsibility.h"

class DatabaseController
{
public:
    explicit DatabaseController(std::shared_ptr<TableSchema> tableSchema= nullptr, DataResponsibility responsibility=DataResponsibility::SELF_ONLY);
    REM_COPY_MOVE(DatabaseController)
    ~DatabaseController();

    void addRelationship(std::shared_ptr<Relationship> relationship, const QString& fieldName, const QString& fieldValue);
    void setArguments(const QMap<QString, QVariant>& args);

    ResultStatus load(QList<QJsonObject>& results);
    ResultStatus add(const QJsonObject& objectJson);
    ResultStatus update(const QJsonObject& jsonObject);
    ResultStatus remove(const QJsonObject& jsonObject);

private:
    class DatabaseControllerImpl;
    std::shared_ptr<DatabaseControllerImpl> impl;
};


#endif //DATABASEOPERATION_H
