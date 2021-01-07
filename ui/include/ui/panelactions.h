#ifndef TROOPTREASURER_ACTIONDEFINITION_H
#define TROOPTREASURER_ACTIONDEFINITION_H

#include <QObject>
#include <QtWidgets>
#include <memory>
#include "ui/panelwindow.h"

class PanelActions
{
public:
    explicit PanelActions(const QJsonObject& definition, QMainWindow *mainwindow);
    virtual ~PanelActions();

    Panel getPanel();
    QList<int> getActionIdentifiers();
    QAction* getAction(int id);

    bool hasMenu() const;
    QMenu* getMenu() const;

    bool hasToolbar() const;
    QToolBar* getToolbar() const;

    void setVisibility(bool visible);

    static QList<PanelActions*> LoadPanelActionDefinitions(QMainWindow *window);
private:
    class PanelActionsImpl;
    std::shared_ptr<PanelActionsImpl> impl;
};

#endif //TROOPTREASURER_ACTIONDEFINITION_H
