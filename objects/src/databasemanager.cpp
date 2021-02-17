#include "objects/databasemanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>

class DatabaseManager::DatabaseManagerImpl
{
public:
    DatabaseManagerImpl(const QString& defFile)
    {
        QFile file1(defFile);
        if(file1.open(QFile::ReadOnly | QFile::Text)){
            QByteArray raw1 = file1.readAll();
            file1.close();
            _databaseDefinition = QJsonDocument::fromJson(raw1).object();
        }
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

    bool verifyTables()
    {
        bool pass = true;
        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        QSqlQuery q(db);
        QString sql = "SELECT TABLE_SCHEMA, TABLE_NAME FROM information_schema.TABLES WHERE TABLE_SCHEMA = database()";
        QList<QString> inDbSchema;
        QString schema;
        if(q.exec(sql))
        {
            while(q.next())
            {
                if(schema.isEmpty())
                {
                    schema = q.value(0).toString();
                }
                inDbSchema.append(q.value(1).toString().toUpper().trimmed());
            }
        }

        QJsonArray tables = _databaseDefinition["database"].toObject()["tables"].toArray();
        for(int i =0; i<tables.count(); i++)
        {
            QJsonObject table = tables[i].toObject();
            if(!inDbSchema.contains(table["name"].toString().toUpper().trimmed()))
            {
                pass = false;
                table["schema"] = schema;
                _notFound.append(table);
            }
        }
        return pass;
    }

    bool verifyViews()
    {
        bool pass = true;
        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        QSqlQuery q(db);
        QString sql = "SELECT TABLE_SCHEMA, TABLE_NAME FROM information_schema.VIEWS WHERE TABLE_SCHEMA = database()";
        QList<QString> inDbSchema;
        QString schema;
        if(q.exec(sql))
        {
            while(q.next())
            {
                if(schema.isEmpty())
                {
                    schema = q.value(0).toString();
                }
                inDbSchema.append(q.value(1).toString().toUpper().trimmed());
            }
        }

        QJsonArray views = _databaseDefinition["database"].toObject()["views"].toArray();
        for(int i =0; i<views.count(); i++)
        {
            QJsonObject view = views[i].toObject();
            if(!inDbSchema.contains(view["name"].toString().toUpper().trimmed()))
            {
                pass = false;
                view["schema"] = schema;
                _notFound.append(view);
            }
        }
        return pass;
    }

    bool verifySchema()
    {
        bool tables = verifyTables();
        bool views = verifyViews();

        return tables==views && tables == true;
    }

    QJsonObject getNotFound() const
    {
        QJsonObject out;
        QJsonArray items;

        for(QJsonObject json : _notFound)
        {
            items.append(json["name"].toString());
        }
        out["not_found"] = items;
        return out;
    }

    QJsonObject initializeDatabaseObjects()
    {
        QJsonObject out;

        for(QJsonObject json: _notFound)
        {
            out[json["name"].toString()] = runDDL(json["ddl"].toString());
        }

        return out;
    }

private:

    bool runDDL(const QString& path)
    {
        QFile file(path);
        QString ddlStmt;
        if(file.open(QFile::ReadOnly | QFile::Text)){
            ddlStmt = QString(file.readAll());
            file.close();
        }
        else
        {
            return false;
        }

        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        QSqlQuery q(db);

        bool r = q.exec(ddlStmt);

        if(!r)
        {
            qDebug() << "Err DB: " << q.lastError().databaseText() << " driver:" << q.lastError().driverText();
        }

        return r;
    }

    QJsonObject _databaseDefinition;
    QList<QJsonObject> _notFound;
};

DatabaseManager::DatabaseManager(const QString &databaseInfoFile): impl(new DatabaseManagerImpl(databaseInfoFile))
{}

DatabaseManager::~DatabaseManager()
{}

DatabaseValidationResponse DatabaseManager::createDatabaseConnection()
{
    return impl->openDatabase();
}

QJsonObject DatabaseManager::initializeDatabaseObjects()
{
    return impl->initializeDatabaseObjects();
}

QJsonObject DatabaseManager::getItemsNotFoundInDatabase() const
{
    return impl->getNotFound();
}

