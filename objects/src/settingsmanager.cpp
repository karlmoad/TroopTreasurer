#include "objects/settingsmanager.h"

class SettingsManager::SettingsManagerImpl
{
public:
    SettingsManagerImpl(QString settingPath, QString configPath)
    {
        QFile file(configPath);
        if(file.open(QFile::ReadOnly | QFile::Text)){
            QByteArray raw = file.readAll();
            file.close();
            _configMeta = QJsonDocument::fromJson(raw).object();
        }

        QFile file1(settingPath);
        if(file1.open(QFile::ReadOnly | QFile::Text)){
            QByteArray raw1 = file1.readAll();
            file1.close();
            _settings = QJsonDocument::fromJson(raw1).object();
        }
        _settingsPath = settingPath;
        init();
    }

    void init()
    {
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
                        // TODO implement KeyManager
                        //section[secureItems[i].toObject()["setting"].toString()] = keymanager->get(generateSecureSettingTag(secureItems[i].toObject()["Section"].toString(),secureItems[i].toObject()["setting"].toString());
                        settings[secureItems[i].toObject()["section"].toString()] = section;
                    }
                }
            }
        }
        _settings = settings;
    }

    bool save(QString *message)
    {
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
                        // TODO implement KeyManager
                        //Over write password value in settings with default value so that we dont save the value to the fs unsecured
                        section[secureItems[i].toObject()["setting"].toString()] = generateSecureSettingTag(secureItems[i].toObject()["Section"].toString(),secureItems[i].toObject()["setting"].toString());
                        settings[secureItems[i].toObject()["section"].toString()] = section;
                    }
                }
            }
        }

        QFile file(_settingsPath);
        if(file.open(QFile::ReadWrite | QFile::Text))
        {
            file.write(QJsonDocument(_settings).toJson());
            file.close();
            return true;
        }
        *message = "Unable to write settings file: " + _settingsPath;
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

private:
    QJsonObject _configMeta;
    QJsonObject _settings;
    QString _settingsPath;

};

SettingsManager* SettingsManager::_instance= nullptr;

SettingsManager::SettingsManager(QString settingPath, QString configMetaPath): impl(new SettingsManagerImpl(settingPath, configMetaPath))
{}

SettingsManager::~SettingsManager()
{
    delete impl;
}

SettingsManager *SettingsManager::getInstance()
{
    return SettingsManager::_instance;
}

SettingsManager *SettingsManager::initialize(QString settingPath, QString configMetaPath)
{
    if(SettingsManager::_instance== nullptr) {
        SettingsManager::_instance = new SettingsManager(settingPath, configMetaPath);
    }
    return SettingsManager::_instance;
}

void SettingsManager::free()
{
    delete SettingsManager::_instance;
}

QJsonObject SettingsManager::getConfigurationMetadata(const QString &key)
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


