#include "objects/settingsmanager.h"
#include "keymanager/keymanager.h"
#include "objects/objecterror.h"
#include <QDebug>

class SettingsManager::SettingsManagerImpl
{
public:
    SettingsManagerImpl(const QString& settingPath,const QString& settingsFile,const QString& configMetaPath)
    {
        QFile file(configMetaPath);
        if(file.open(QFile::ReadOnly | QFile::Text)){
            QByteArray raw = file.readAll();
            file.close();
            _configMeta = QJsonDocument::fromJson(raw).object();
        }
        else
        {
            ObjectError err("Unable to open and read configuration metadata file", static_cast<int>(ObjectErrorCode::ERROR_READ_FILE));
            err.raise();
        }

        _settingsPath = settingPath;
        _settingsFile = QString(settingPath + "/" + settingsFile);
        QDir dir(_settingsPath);
        //check if settings directory exists iuf not create it
        if(!dir.exists())
        {
            dir.mkpath(".");
        }

        QFile file1(_settingsFile);
        if(file1.open(QFile::ReadOnly | QFile::Text)){
            QByteArray raw1 = file1.readAll();
            file1.close();
            _settings = QJsonDocument::fromJson(raw1).object();
        }
        else
        {
            _settings = QJsonObject();
        }

        init();
    }

    void init()
    {
        KeyManager kmgr;
        QJsonObject settings = _settings;
        QJsonArray secureItems = _configMeta["secured"].toArray();
        for(int i =0; i<secureItems.count(); i++)
        {
            if (settings.contains(secureItems[i].toObject()["section"].toString()))
            {
                QJsonObject section = settings[secureItems[i].toObject()["section"].toString()].toObject();
                if(section.contains(secureItems[i].toObject()["setting"].toString()))
                {
                    QString secured = section[secureItems[i].toObject()["setting"].toString()].toString();
                    if (secured.trimmed().size() > 0)
                    {
                        QString securedValue;
                        if(kmgr.getValue(generateSecureSettingTag(secureItems[i].toObject()["Section"].toString(),secureItems[i].toObject()["setting"].toString()), securedValue))
                        {
                            section[secureItems[i].toObject()["setting"].toString()] = securedValue;
                            settings[secureItems[i].toObject()["section"].toString()] = section;
                        }
                        else
                        {
                            ObjectError err(kmgr.lastError().getErrorMessage(), kmgr.lastError().getErrorCode());
                            err.raise();
                        }
                    }
                }
            }
        }

        _settings = settings;
    }

    bool save(QString *message)
    {
        KeyManager kmgr;
        QJsonObject settings = _settings;
        QJsonArray secureItems = _configMeta["secured"].toArray();
        for(int i =0; i<secureItems.count(); i++)
        {
            if (settings.contains(secureItems[i].toObject()["section"].toString()))
            {
                QJsonObject section = settings[secureItems[i].toObject()["section"].toString()].toObject();
                if(section.contains(secureItems[i].toObject()["setting"].toString()))
                {
                    QString secured = section[secureItems[i].toObject()["setting"].toString()].toString();
                    if (secured.trimmed().size() > 0)
                    {
                        //Over write password value in settings with default value so that we dont save the value to the fs unsecured
                        if(kmgr.setValue(generateSecureSettingTag(secureItems[i].toObject()["Section"].toString(),secureItems[i].toObject()["setting"].toString()), secured))
                        {
                            section[secureItems[i].toObject()["setting"].toString()] = generateSecureSettingTag(secureItems[i].toObject()["Section"].toString(),secureItems[i].toObject()["setting"].toString());
                            settings[secureItems[i].toObject()["section"].toString()] = section;
                        }
                        else
                        {
                            ObjectError err(kmgr.lastError().getErrorMessage(), kmgr.lastError().getErrorCode());
                            err.raise();
                        }
                    }
                }
            }
        }

        QFile file(_settingsFile);
        if(file.open(QFile::ReadWrite | QFile::Text))
        {
            file.write(QJsonDocument(settings).toJson());
            file.close();
            return true;
        }
        *message = "Unable to write settings file: " + _settingsPath + " Error: " + file.errorString();
        return false;
    }

    QString generateSecureSettingTag(const QString& section, const QString& setting)
    {
        return QString("%1-%2-%3-secured").arg(_configMeta["application_id"].toString(),section, setting);
    }

    void setSettingsSegment(const QString& key, QJsonObject data)
    {
        _settings[key] = data;
    }

    QJsonObject getSettingsSegment(const QString& key)
    {
        if(_settings.contains(key))
        {
            return _settings[key].toObject();
        }
        return QJsonObject();
    }

    QJsonObject getConfigurationMetadata(const QString& key)
    {
        if(_configMeta.contains(key))
        {
            return _configMeta[key].toObject();
        }
        return QJsonObject();
    }

    bool initDB()
    {
        QJsonObject settings = getSettingsSegment("database");
        if(settings.isEmpty()) return false;

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
            return false;
        }

        return true;
    }

    void closeDB()
    {
        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        if(db.open())
        {
            db.close();
        }
    }

private:
    QJsonObject _configMeta;
    QJsonObject _settings;
    QString _settingsPath;
    QString _settingsFile;

};

SettingsManager* SettingsManager::_instance= nullptr;

SettingsManager::SettingsManager(const QString& settingPath,const QString& settingsFile,const QString& configMetaPath): impl(new SettingsManagerImpl(settingPath,settingsFile,configMetaPath))
{}

SettingsManager::~SettingsManager()
{
    delete impl;
}

SettingsManager *SettingsManager::getInstance()
{
    return SettingsManager::_instance;
}

SettingsManager *SettingsManager::initialize(const QString& settingPath,const QString& settingsFile,const QString& configMetaPath)
{
    if(SettingsManager::_instance== nullptr) {
        SettingsManager::_instance = new SettingsManager(settingPath, settingsFile, configMetaPath);
    }
    return SettingsManager::_instance;
}

void SettingsManager::free()
{
    _instance->impl->closeDB();
    delete SettingsManager::_instance;
}

QJsonObject SettingsManager::getConfigurationSectionMetadata(const QString &key)
{
    return impl->getConfigurationMetadata(key);
}

QJsonObject SettingsManager::getSettingsSegment(const QString &key)
{
    return impl->getSettingsSegment(key);
}

void SettingsManager::setSettingsSegment(QJsonObject data, const QString &key)
{
    impl->setSettingsSegment(key, data);
}

bool SettingsManager::saveSettings(QString *message)
{
    return impl->save(message);
}

bool SettingsManager::initializeDatabaseConnection()
{
    return impl->initDB();
}


