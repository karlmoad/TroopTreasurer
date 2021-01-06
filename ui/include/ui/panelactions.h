#ifndef TROOPTREASURER_ACTIONDEFINITION_H
#define TROOPTREASURER_ACTIONDEFINITION_H

#include <QObject>
#include <QtWidgets>
#include <memory>
#include "ui/panelwindow.h"

class PanelActions: public QObject
{
    Q_OBJECT
public:
    explicit PanelActions(QMenuBar *menubar, QToolBar *toolbar, const QJsonObject& definition, QObject *parent=nullptr);
    virtual ~PanelActions();

    Panel getPanel();
    QList<int> getActionIdentifiers();
    QAction* getAction(int id);
    void setVisibility(bool visible);

    static QList<PanelActions*> LoadPanelActionDefinitions(QMenuBar *menubar, QToolBar *toolbar, QObject *parent=nullptr);
private:
    class PanelActionsImpl;
    std::shared_ptr<PanelActionsImpl> impl;
};

#endif //TROOPTREASURER_ACTIONDEFINITION_H
