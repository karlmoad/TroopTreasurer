#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QIcon>
#include <QString>
#include <QMap>
#include <QList>

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
    void NotifyContextItemActions(bool actionNew, bool actionEdit, bool actionDelete);
    void NotifyContextSavable(bool savable);
    void PanelCloseHandler(int panel);

signals:
    void SaveItemInContext();
    void NewItemInContext();
    void EditItemInContext();
    void DeleteItemInContext();

protected:
    virtual void closeEvent(QCloseEvent *event) override;


private:
    void init();
    void OpenFundsManagementPanel();
    void ConnectPanel(QWidget *panel, const QString& header);
private:
    Ui::MainWindow *ui;
    QMenuBar *mainMenuBar;
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
    QMap<int,QList<QAction>> panelMenus;
};

#endif // MAINWINDOW_H
