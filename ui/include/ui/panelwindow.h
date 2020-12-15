//
// Created by Karl Moad on 12/15/20.
//

#ifndef TROOPTREASURER_PANELWINDOW_H
#define TROOPTREASURER_PANELWINDOW_H

#include <QAction>
#include <QIcon>
#include <QString>
#include <QList>

class PanelWindow: public QObject
{
    Q_OBJECT
public:
    explicit PanelWindow(QObject *parent=nullptr): QObject(parent){};
    virtual ~PanelWindow()=default;

    virtual QList<QAction> menuItems()=0;
    virtual QList<QAction> toolbarItems()=0;

signals:
    virtual void itemActionStateChange(bool actionNew, bool actionEdit, bool actionDelete)=0;
    virtual void saveActionStateChanged(bool savable)=0;

public slots:
    virtual void newItemHandler()=0;
    virtual void editItemHandler()=0;
    virtual void deleteItemHandler()=0;
    virtual void saveItemHandler()=0;
};

#endif //TROOPTREASURER_PANELWINDOW_H
