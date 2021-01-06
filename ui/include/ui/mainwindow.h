/*
 * TroopTreasurer UI
 * Author: Karl Moad
 * ---------------------------------
 * MainWindow is the primary application window that displays
 * all child panels of user functionality.
 *
 * Responsible for maintaining:
 *  Main toolbar and menu objects and state
 *  Active and inactive panels
 *      - signals registration and de-registration on active state
 *      change of a child panel
 *
 *
 * Window also instantiates the following singleton objects:
 *  SettingsManager
*/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <memory>
#include "itemstate.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void ContextItemStateChangeHandler(ItemState state);
    void PanelCloseHandler(int panelIdx);
    void ActivePanelChanged(int index);
    void SettingsHandler();

signals:
    void ContextItemActionTriggered(ItemAction action);

protected:
    virtual void closeEvent(QCloseEvent *event) override;


private:
    class MainWindowImpl;
    std::shared_ptr<MainWindowImpl> impl;
};

#endif // MAINWINDOW_H
