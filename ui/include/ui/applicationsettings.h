//
// Created by Karl Moad on 12/21/20.
//

#ifndef TROOPTREASURER_APPLICATIONSETTINGS_H
#define TROOPTREASURER_APPLICATIONSETTINGS_H

#include <QString>
#include <QIcon>

class DatabaseSettingsPane;

enum class ApplicationSettingsType
{
    GENERAL,
    DATABASE,
    UNKNOWN
};


class ApplicationSettingsUtility
{
public:
    static QString ApplicationSettingTypeToString(ApplicationSettingsType setting);
    static ApplicationSettingsType StringToApplicationSettingsType(const QString& value);
    static QIcon ApplicationSettingsTypeIcon(ApplicationSettingsType type);
    static QList<ApplicationSettingsType> ApplicationSettingsTypeList();
    static QWidget* Factory(ApplicationSettingsType type, QWidget *parent= nullptr);
};


















#endif //TROOPTREASURER_APPLICATIONSETTINGS_H
