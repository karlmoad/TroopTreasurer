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
#include <QString>


class SettingsManager
{
public:
    SettingsManager(QString settingPath, QString configMetaPath);
    ~SettingsManager();
    static SettingsManager *getInstance();
    static SettingsManager *initialize(QString settingPath, QString configMetaPath);
    static void free();
    QJsonObject getConfigurationSectionMetadata(const QString& key);
    QJsonObject getSettingsSegment(QString const &key);
    void setSettingsSegment(QJsonObject data, const QString& key);
    bool saveSettings(QString *message);

private:
    class SettingsManagerImpl;
    SettingsManagerImpl *impl;
    static SettingsManager *_instance;
};


#endif //TROOPTREASURER_SETTINGSMANAGER_H
