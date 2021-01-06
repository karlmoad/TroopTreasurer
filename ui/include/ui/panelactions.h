#ifndef TROOPTREASURER_ACTIONDEFINITION_H
#define TROOPTREASURER_ACTIONDEFINITION_H

#include <QObject>
#include <QtWidgets>
#include "ui/panelwindow.h"

class PanelActions: public QObject
{
    Q_OBJECT
public:
    explicit PanelActions(QObject *parent= nullptr);
    virtual ~PanelActions();

    Panel getPanel();
    QAction* getAction(int id);
    void setVisibility(bool visible);

    static QList<ActionDefinition*> LoadPanelActionDefinitions(QMenuBar *menubar, QToolBar *toolbar, QObject *parent=nullptr);
private:
    class PanelActionsImpl;
    PanelActionsImpl *impl;
}

#endif //TROOPTREASURER_ACTIONDEFINITION_H
