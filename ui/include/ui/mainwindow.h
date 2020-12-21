#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QIcon>
#include <QString>
#include <QMap>
#include <QList>
#include "panelfactory.h"
#include "panelwindow.h"
#include "itemstate.h"

namespace Ui
{
    class MainWindow;
}

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

signals:
    void ContextItemActionTriggered(ItemAction action);

protected:
    virtual void closeEvent(QCloseEvent *event) override;


private:
    void init();
    void OpenImportTemplateEditorPanel();
    QMenu* getMenu(const QString &text);
    void initNewPanel(Panel panel);
    void setPanelMenuToolbarVisibility(PanelWindow *panel, int index, bool visible);
    void activatePanel(PanelWindow *panel, int index);
    void deactivatePanel(PanelWindow *panel, int index);
    void unregisterPanel(PanelWindow *panel, int index);
    void registerPanel(PanelWindow *panel, int index);


private:
    Ui::MainWindow *ui;
    QToolBar *mainToolbar;
    QMenu *mnuFile;
    QMenu *mnuAbout;
    QMenu *mnuEdit;
    QMenu *mnuFileOpenSubmenu;
    QMenu *mnuFileImportSubmenu;
    QAction *actSave;
    QAction *actOpenAccounts;
    QAction *actImportEditTemplates;
    QAction *actImportData;
    QAction *actSettings;
    QAction *actQuit;
    QAction *actAbout;
    QAction *actEditItem;
    QAction *actAddItem;
    QAction *actDeleteItem;
    QAction *actFundsManagement;
    QMap<int,QList<QAction*>> panelMenus;
    QMap<int,QList<QAction*>> panelToolbarItems;
    QMap<Panel, QList<int>> panel2Index;
    QMap<int, PanelWindow*> index2Panel;
    int currentActivePanelIndex=-1;
};

#endif // MAINWINDOW_H
