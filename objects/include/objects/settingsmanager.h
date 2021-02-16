/*
 * TroopTreasurer
 * Author: Karl Moad
 * ---------------------------------
 * SettingsManager - Singleton object that
 * manages the persistence and extraction of
 * user application settings from a file
 * directory location.
 *
*/

#ifndef TROOPTREASURER_SETTINGSMANAGER_H
#define TROOPTREASURER_SETTINGSMANAGER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QDir>
#include <QString>
#include <QSqlDatabase>
#include <QSqlError>

class SettingsManager
{
public:
    SettingsManager(const QString& settingPath,const QString& settingsFile,const QString& configMetaPath);
    ~SettingsManager();
    QJsonObject getConfigurationSectionMetadata(const QString& key);
    QJsonObject getSettingsSegment(QString const &key);
    void setSettingsSegment(QJsonObject data, const QString& key);
    bool saveSettings(QString *message);


public:
    static SettingsManager *getInstance();
    static SettingsManager *initialize(const QString& settingPath,const QString& settingsFile,const QString& configMetaPath);
    static void free();

private:
    bool initializeDatabaseConnection();
    class SettingsManagerImpl;
    SettingsManagerImpl *impl;
    static SettingsManager *_instance;
};


#endif //TROOPTREASURER_SETTINGSMANAGER_H
