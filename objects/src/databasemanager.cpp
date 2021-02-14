#include "objects/databasemanager.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

class DatabaseManager::DatabaseManagerImpl
{
public:
    DatabaseManagerImpl(const QString& defFile): _databaseDefinitionFile(defFile)
    {

    }

    virtual ~DatabaseManagerImpl()
    {
        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        db.close();
    }

    DatabaseValidationResponse openDatabase()
    {
        QJsonObject settings = SettingsManager::getInstance()->getSettingsSegment("database");;
        if(settings.isEmpty()) return DatabaseValidationResponse::INVALID_NONE_SETTINGS;

        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL","DATABASE");
        db.setDatabaseName(settings["schema"].toString(""));
        db.setHostName(settings["host"].toString(""));
        db.setPort(settings["port"].toInt(3306));
        db.setUserName(settings["user"].toString(""));
        db.setPassword(settings["password"].toString(""));
        QString opts;
        opts.append(QString("SSL_CA=%1;").arg(settings["ca_crt"].toString("")));
        opts.append(QString("SSL_CERT=%1;").arg(settings["svr_crt"].toString("")));
        opts.append(QString("SSL_KEY=%1;").arg(settings["svr_key"].toString("")));
        db.setConnectOptions(opts);

        if(!db.open())
        {
            return DatabaseValidationResponse::COULD_NOT_CONNECT;
        }

        if(verifySchema())
        {
            return DatabaseValidationResponse::OPEN;
        }
        else
        {
            return DatabaseValidationResponse::SCHEMA_NOT_INITIALIZED;
        }
    }

    bool verifySchema()
    {


    }

    bool initializeSchema()
    {

    }

private:
    QString _databaseDefinitionFile;
};

DatabaseManager::DatabaseManager(const QString &databaseInfoFile): impl(new DatabaseManagerImpl(databaseInfoFile))
{}

DatabaseManager::~DatabaseManager()
{}

DatabaseValidationResponse DatabaseManager::createDatabaseConnection()
{
    return impl->openDatabase();
}

bool DatabaseManager::initializeSchema()
{
    return impl->initializeSchema();
}
