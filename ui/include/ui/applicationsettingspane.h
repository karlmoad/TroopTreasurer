//
// Created by Karl Moad on 12/21/20.
//

#ifndef TROOPTREASURER_APPLICATIONSETTINGSPANE_H
#define TROOPTREASURER_APPLICATIONSETTINGSPANE_H

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "applicationsettings.h"


class ApplicationSettingsPane
{
public:
    virtual ApplicationSettingsType settingsType()=0;
    virtual void loadSettings(const QJsonObject& settings)=0;
    virtual QJsonObject settings()=0;
    virtual void save()=0;
};


#endif //TROOPTREASURER_APPLICATIONSETTINGSPANE_H
