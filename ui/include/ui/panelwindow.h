/*
 * TroopTreasurer UI
 * Author: Karl Moad
 * ---------------------------------
 *  PanelWindow Class
 *
 *  Abstract base class for all windows that will be presented as
 *  functional panels for the user to interact within the main window
 *  of the application.  Class contains methods to define contextual
 *  panel menus and toolbar items wile also providing the signal linkage
 *  base for action declared and controlled by the main window
 *
 */

#ifndef TROOPTREASURER_PANELWINDOW_H
#define TROOPTREASURER_PANELWINDOW_H

#include <QAction>
#include <QIcon>
#include <QString>
#include <QList>
#include <QMenu>
#include <QToolBar>
#include "itemstate.h"

class PanelActions;

enum class Panel
{
    UNDEFINED=0,
    IMPORT_TEMPLATE_EDITOR=1,
};

class PanelWindow: public QWidget
{
    Q_OBJECT
public:
    explicit PanelWindow(QWidget *parent= nullptr);
    virtual ~PanelWindow();
    virtual QString panelName() const = 0;
    virtual Panel panelId() const = 0;
    virtual ItemState getCurrentState() const = 0;
    virtual void activate(PanelActions *actions)=0;
    virtual void deactivate(PanelActions *actions)=0;

signals:
    void itemActionStateChange(ItemState state);

public slots:
    virtual void itemActionHandler(ItemAction action)=0;

};

#endif //TROOPTREASURER_PANELWINDOW_H
