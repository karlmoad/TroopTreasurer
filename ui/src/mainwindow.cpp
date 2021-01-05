#include "ui/mainwindow.h"
#include "ui_mainwindow.h"
#include <QIcon>
#include <QString>
#include <QMap>
#include <QList>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "ui/panelfactory.h"
#include "ui/panelwindow.h"
#include "ui/applicationsettingsdialog.h"
#include "ui/applicationconstants.h"
#include "objects/settingsmanager.h"
#include "objects/objecterror.h"

class MainWindow::MainWindowImpl
{
public:
    MainWindowImpl(MainWindow *w): window(w), ui(new Ui::MainWindow)
    {
        ui->setupUi(window);
        window->setUnifiedTitleAndToolBarOnMac(true);
        loadSettings();
    }

    ~MainWindowImpl()
    {
        SettingsManager::free();
        delete ui;
    }

    void init()
    {
        initMenuAndToolbar();
    }

    void closePanel(int panelIdx)
    {
        PanelWindow *p = dynamic_cast<PanelWindow*>(ui->tabMain->widget(panelIdx));
        deactivatePanel(p,panelIdx);
        ui->tabMain->removeTab(panelIdx);
        p->close();
        unregisterPanel(p,panelIdx);

        if(ui->tabMain->count() ==0)
        {
            defaultState();
        }
        delete(p);
    }

    void setContextState(ItemState state)
    {
        actSave->setEnabled(state.saveEnabled());
        actAddItem->setEnabled(state.addEnabled());
        actEditItem->setEnabled(state.editEnabled());
        actDeleteItem->setEnabled(state.deleteEnabled());
    }

    void setActivePanel(int index)
    {
        if(index >=0)
        {
            PanelWindow *p = index2Panel[index];
            if (p)
            {
                activatePanel(p, index);
            }
        }
    }

    void editSettings()
    {
        ApplicationSettingsDialog *appSettings = new ApplicationSettingsDialog(window);
        appSettings->setModal(true);
        appSettings->show();
    }

private:

    void initMenuAndToolbar()
    {
        mnuFile = window->menuBar()->addMenu("&File");
        mnuEdit = window->menuBar()->addMenu("&Edit");
        mnuAbout = window->menuBar()->addMenu("&About");

        //File menu
        mnuFileOpenSubmenu = mnuFile->addMenu("Open");
        actFundsManagement = new QAction("Funds Management", this->window);
        mnuFileOpenSubmenu->addAction(actFundsManagement);
        actFundsManagement->setStatusTip("Open funds management panel");

        mainToolbar = window->addToolBar("MAIN");
        actSave = new QAction(QIcon(":/resources/disk.png"), "&Save", this->window);
        actSave->setStatusTip(tr("Save Item"));
        actSave->setEnabled(false);
        mainToolbar->addAction(actSave);
        mnuFile->addAction(actSave);
        connect(actSave, &QAction::triggered, [this](){
            emit window->ContextItemActionTriggered(ItemAction::SAVE);
        });
        defaultActions.append(actSave);


        mnuFileImportSubmenu = mnuFile->addMenu("Import");
        actImportEditTemplates = new QAction("Edit Templates", this->window);
        actImportEditTemplates->setStatusTip("Edit a data import template");
        connect(actImportEditTemplates, &QAction::triggered, [this](){
            OpenImportTemplateEditorPanel();
        });
        defaultActions.append(actImportEditTemplates);

        actImportData = new QAction("Import Data", window);
        actImportData->setStatusTip("Import a data file");
        mnuFileImportSubmenu->addAction(actImportEditTemplates);
        mnuFileImportSubmenu->addAction(actImportData);

        defaultActions.append(actImportData);

        actSettings = new QAction("Settings",window);
        mnuFile->addAction(actSettings);
        defaultActions.append(actSettings);
        connect(actSettings, &QAction::triggered, window, &MainWindow::SettingsHandler);

        mnuFile->addSeparator();
        actQuit = new QAction("&Quit", window);
        mnuFile->addAction(actQuit);

        defaultActions.append(actQuit);

        actAddItem = new QAction(QIcon(":/resources/page_white_add.png"),"Add item", window);
        actAddItem->setStatusTip("Add new item to current context");
        actAddItem->setEnabled(false);
        mnuEdit->addAction(actAddItem);
        mainToolbar->addAction(actAddItem);
        connect(actAddItem, &QAction::triggered, [this](){
            emit window->ContextItemActionTriggered(ItemAction::ADD);
        });
        defaultActions.append(actAddItem);

        actEditItem = new QAction(QIcon(":/resources/page_white_edit.png"),"Edit item", window);
        actEditItem->setStatusTip("Edit the current context selected item");
        actEditItem->setEnabled(false);
        mnuEdit->addAction(actEditItem);
        mainToolbar->addAction(actEditItem);
        connect(actEditItem, &QAction::triggered, [this](){
            emit window->ContextItemActionTriggered(ItemAction::EDIT);
        });
        defaultActions.append(actEditItem);

        actDeleteItem = new QAction(QIcon(":/resources/page_white_delete.png"),"Delete item",window);
        actDeleteItem->setStatusTip("Delete the current context selected item");
        actDeleteItem->setEnabled(false);
        mnuEdit->addAction(actDeleteItem);
        mainToolbar->addAction(actDeleteItem);
        connect(actDeleteItem, &QAction::triggered, [this](){
            emit window->ContextItemActionTriggered(ItemAction::DELETE);
        });
        defaultActions.append(actDeleteItem);

        actSep = mainToolbar->addSeparator();
        defaultActions.append(actSep);

        actAbout = new QAction("About", window);
        actAbout->setStatusTip("About this program");
        mnuAbout->addAction(actAbout);
        defaultActions.append(actAbout);

        connect(actAbout, &QAction::triggered, [this](){
            qDebug() << "About clicked";
        });

        //wire panels container control events
        connect(ui->tabMain, &QTabWidget::currentChanged, window, &MainWindow::ActivePanelChanged);
        connect(ui->tabMain, &QTabWidget::tabCloseRequested, window, &MainWindow::PanelCloseHandler);

        loadMainMenuPanelEntries();
    }

    void loadMainMenuPanelEntries()
    {
        //create and register panel main menu entries
        QFile file(":/resources/files/panels.json");
        if(file.open(QFile::ReadOnly | QFile::Text)){
            QByteArray raw = file.readAll();
            file.close();
            QJsonObject panelConfig = QJsonDocument::fromJson(raw).object();
            QJsonArray panels = panelConfig["panels"].toArray();
            for(int i=0; i<panels.count(); i++)
            {
                QJsonObject panel = panels[i].toObject();
                registerMainMenuPanelEntry(panel["id"].toInt(), panel["menu"].toString());
            }
        }
        else
        {
            ObjectError err("Unable to open and read main panel registration file", static_cast<int>(ObjectErrorCode::ERROR_READ_FILE));
            err.raise();
        }
    }

    void registerMainMenuPanelEntry(int panelId, const QString& display)
    {
        QMenu* pmnu = window->menuBar()->addMenu(display);
        pmnu->setHidden(true);
        mainMenuRegistry.insert(panelId, pmnu);
    }

    void loadSettings()
    {
        settingsManager = SettingsManager::initialize(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation),APP::ApplicationSettingsFile,APP::ApplicationConfigMetadataFile);
    }

    void OpenImportTemplateEditorPanel()
    {
        initNewPanel(Panel::IMPORT_TEMPLATE_EDITOR);
    }

    QMenu* getMenu(Panel panel)
    {
        if(mainMenuRegistry.contains(static_cast<int>(panel)))
        {
            return mainMenuRegistry[static_cast<int>(panel)];
        }
        return nullptr;
    }

    void initNewPanel(Panel panel)
    {
        PanelWindow *p = PanelFactory::CreateNewPanel(panel,window);
        int tabidx = ui->tabMain->addTab(p,p->panelName());

        //add panel to index
        if(panel2Index.contains(panel))
        {
            panel2Index[panel].append(tabidx);
        }
        else
        {
            panel2Index.insert(panel, QList<int>({tabidx}));
        }
        registerPanel(p,tabidx);
        ui->tabMain->setCurrentIndex(tabidx);
        activatePanel(p,tabidx);
    }

    void setPanelMenuVisibility(PanelWindow *panel, bool visible)
    {
        if(panel)
        {
            qDebug() << "Panel: " << panel->panelName() << " Visible: " << visible;
            QMenu *menu = getMenu(panel->panelId());
            if (menu)
            {
                menu->setVisible(visible);
            }
        }
    }

    void activatePanel(PanelWindow *panel, int index)
    {
        qDebug() << "Activate panel called";
        connect(panel, &PanelWindow::itemActionStateChange, window, &MainWindow::ContextItemStateChangeHandler);
        connect(window, &MainWindow::ContextItemActionTriggered, panel, &PanelWindow::itemActionHandler);
        setPanelMenuVisibility(panel,true);
        panel->activate();
    }

    void deactivatePanel(PanelWindow *panel, int index)
    {
        qDebug() << "Deactivate panel called";
        disconnect(panel, nullptr, window, nullptr);
        disconnect(window, nullptr, panel, nullptr);
        setPanelMenuVisibility(panel,false);
        panel->deactivate();
    }

    void unregisterPanel(PanelWindow *panel, int index)
    {
        qDebug() << "Unregister panel called";
        if(panel == nullptr) return;
        QMenu *menu = getMenu(panel->panelId());
        panel->unregisterPanel(menu, mainToolbar);
        index2Panel.remove(index);
    }

    void registerPanel(PanelWindow *panel, int index)
    {
        qDebug() << "Register panel called";
        index2Panel[index]=panel;
        QMenu *menu = getMenu(panel->panelId());
        panel->registerPanel(menu, mainToolbar);
    }

    void defaultState()
    {
        actSave->setEnabled(false);
        actEditItem->setEnabled(false);
        actAddItem->setEnabled(false);
        actDeleteItem->setEnabled(false);

        QList<QAction *> actions = mainToolbar->findChildren<QAction *>(QString(), Qt::FindDirectChildrenOnly);
        for (int i = 0; i < actions.count(); i++)
        {
            if (!defaultActions.contains(actions[i]))
            {
                mainToolbar->removeAction(actions[i]);
            }
        }

        for(int panelId: mainMenuRegistry.keys())
        {
            mainMenuRegistry[panelId]->setVisible(false);
        }
    }

private:
    MainWindow *window;
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
    QAction *actSep;
    QAction *actFundsManagement;
    QMap<Panel, QList<int>> panel2Index;
    QMap<int, PanelWindow*> index2Panel;
    QMap<int, QMenu*> mainMenuRegistry;
    QList<QAction*> defaultActions;
    SettingsManager *settingsManager;
};

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), impl(new MainWindowImpl(this))
{
    impl->init();
}

MainWindow::~MainWindow()
{
    delete impl;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //QSqlDatabase db = QSqlDatabase::database();
    //if(db.isOpen())
    //{
    //    db.close();
    //}
    QWidget::closeEvent(event);
    event->accept();
}

void MainWindow::ContextItemStateChangeHandler(ItemState state)
{
    impl->setContextState(state);
}

void MainWindow::PanelCloseHandler(int panelIdx)
{
    impl->closePanel(panelIdx);
}

void MainWindow::ActivePanelChanged(int index)
{
    impl->setActivePanel(index);
}

void MainWindow::SettingsHandler()
{
    impl->editSettings();
}


