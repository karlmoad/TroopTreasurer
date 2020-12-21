//
// Created by Karl Moad on 12/21/20.
//

#include "ui/applicationsettings.h"
#include "ui/databasesettingspane.h"
#include "ui/applicationsettingspane.h"

QString ApplicationSettingsUtility::ApplicationSettingTypeToString(ApplicationSettingsType setting)
{
    switch(setting)
    {
        case ApplicationSettingsType::GENERAL:
            return "general";
            break;
        case ApplicationSettingsType::DATABASE:
            return "database";
            break;
        default:
            return "unknown";
            break;
    }
}

ApplicationSettingsType ApplicationSettingsUtility::StringToApplicationSettingsType(const QString &value)
{
    if(value.compare("general", Qt::CaseSensitive) == 0)
    {
        return ApplicationSettingsType::GENERAL;
    }

    if(value.compare("database",Qt::CaseSensitive) ==0)
    {
        return ApplicationSettingsType::DATABASE;
    }

    return ApplicationSettingsType::UNKNOWN;
}

QIcon ApplicationSettingsUtility::ApplicationSettingsTypeIcon(ApplicationSettingsType type)
{
    switch(type)
    {
        case ApplicationSettingsType::DATABASE:
            return QIcon(":/resources/database.png");
            break;
        default:
            return QIcon();
            break;
    }
}

QList<ApplicationSettingsType> ApplicationSettingsUtility::ApplicationSettingsTypeList()
{
    return QList<ApplicationSettingsType>({ApplicationSettingsType::DATABASE});
}

QWidget* ApplicationSettingsUtility::Factory(ApplicationSettingsType type, QWidget *parent)
{
    switch(type)
    {
        case ApplicationSettingsType::DATABASE:
        {
            return new DatabaseSettingsPane(parent);
            break;
        }
        default:
        {
            return nullptr;
            break;
        }
    }
}
