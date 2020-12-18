//
// Created by Karl Moad on 12/15/20.
//

#ifndef TROOPTREASURER_PANELWINDOW_H
#define TROOPTREASURER_PANELWINDOW_H

#include <QAction>
#include <QIcon>
#include <QString>
#include <QList>
#include "itemstate.h"

class PanelWindow
{
public:
    virtual const QString& panelMenuText() const =0;
    virtual const QList<QAction*>& menuItems() const =0;
    virtual const QList<QAction*>& toolbarItems() const =0;
    virtual ItemState getCurrentState() const =0;
    virtual void itemActionStateChange(ItemState state)=0;
    virtual void itemActionHandler(ItemAction action)=0;
    virtual bool allowMultipleInstances()=0;
};

#endif //TROOPTREASURER_PANELWINDOW_H
