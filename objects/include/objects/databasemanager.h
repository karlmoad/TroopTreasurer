#ifndef TROOPTREASURER_DATABASEMANAGER_H
#define TROOPTREASURER_DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlResult>
#include <memory>
#include "settingsmanager.h"

enum class DatabaseValidationResponse
{
    UNKNONW = 0,
    OPEN = 1,
    INVALID_NONE_SETTINGS=2,
    COULD_NOT_CONNECT=3,
    SCHEMA_NOT_INITIALIZED=4
};

class DatabaseManager
{
public:
    DatabaseManager(const QString& databaseDefinitionFile);
    ~DatabaseManager();

    DatabaseValidationResponse createDatabaseConnection();
    bool initializeSchema();

private:
    class DatabaseManagerImpl;
    std::shared_ptr<DatabaseManagerImpl> impl;
};


#endif //TROOPTREASURER_DATABASEMANAGER_H
