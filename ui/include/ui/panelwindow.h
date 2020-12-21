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

class PanelWindow: public QWidget
{
    Q_OBJECT
public:
    explicit PanelWindow(QWidget *parent= nullptr);
    virtual ~PanelWindow();
    virtual QString panelName() const = 0;
    virtual const QString& panelMenuText() const = 0;
    virtual const QList<QAction*>& menuItems() const = 0;
    virtual const QList<QAction*>& toolbarItems() const = 0;
    virtual ItemState getCurrentState() const = 0;
    virtual bool hasMenu() const = 0;
    virtual bool hasToolbarItems() const = 0;

signals:
    void itemActionStateChange(ItemState state);

public slots:
    virtual void itemActionHandler(ItemAction action)=0;

};

#endif //TROOPTREASURER_PANELWINDOW_H
