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
#include "ui/panelactions.h"
#include "ui/applicationsettingsdialog.h"
#include "ui/applicationconstants.h"
#include "ui/importdatadialog.h"
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
        initDatabase();
        window->setWindowTitle("Troop Treasurer");
    }

    ~MainWindowImpl()
    {
        SettingsManager::free();
        qDeleteAll(panelActionRegistry);
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
            if(p)
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
        mnuAccounts = window->menuBar()->addMenu("Accounts");
        mnuFunds = window->menuBar()->addMenu("Bank");

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

        mnuFileImportSubmenu = mnuFile->addMenu("Import");
        actImportEditTemplates = new QAction("Edit Templates", this->window);
        actImportEditTemplates->setStatusTip("Edit a data import template");
        connect(actImportEditTemplates, &QAction::triggered, [this](){
            initNewPanel(Panel::IMPORT_TEMPLATE_EDITOR);
        });

        actImportData = new QAction("Import Data", window);
        actImportData->setStatusTip("Import a data file");
        mnuFileImportSubmenu->addAction(actImportEditTemplates);
        mnuFileImportSubmenu->addAction(actImportData);
        connect(actImportData, &QAction::triggered, [this](){
            QStringList paths = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation);
            QString filename = QFileDialog::getOpenFileName(window,tr("Import CSV file"), (paths.size() > 0 ? paths[0]: ""),tr("Comma Separated Values File (*.csv);;All Files (*)"));
            if(!filename.isEmpty())
            {
                ImportDataDialog *importDialog = new ImportDataDialog(filename, window);
                importDialog->setModal(true);
                importDialog->exec();
            }
        });

        actSettings = new QAction("Settings",window);
        mnuFile->addAction(actSettings);
        connect(actSettings, &QAction::triggered, window, &MainWindow::SettingsHandler);

        mnuFile->addSeparator();
        actQuit = new QAction("&Quit", window);
        mnuFile->addAction(actQuit);

        actAddItem = new QAction(QIcon(":/resources/page_white_add.png"),"Add", window);
        actAddItem->setStatusTip("Add new item to current context");
        actAddItem->setEnabled(false);
        mnuEdit->addAction(actAddItem);
        mainToolbar->addAction(actAddItem);
        connect(actAddItem, &QAction::triggered, [this](){
            emit window->ContextItemActionTriggered(ItemAction::ADD);
        });

        actEditItem = new QAction(QIcon(":/resources/page_white_edit.png"),"Edit", window);
        actEditItem->setStatusTip("Edit the current context selected item");
        actEditItem->setEnabled(false);
        mnuEdit->addAction(actEditItem);
        mainToolbar->addAction(actEditItem);
        connect(actEditItem, &QAction::triggered, [this](){
            emit window->ContextItemActionTriggered(ItemAction::EDIT);
        });

        actDeleteItem = new QAction(QIcon(":/resources/page_white_delete.png"),"Delete",window);
        actDeleteItem->setStatusTip("Delete the current context selected item");
        actDeleteItem->setEnabled(false);
        mnuEdit->addAction(actDeleteItem);
        mainToolbar->addAction(actDeleteItem);
        connect(actDeleteItem, &QAction::triggered, [this](){
            emit window->ContextItemActionTriggered(ItemAction::DELETE);
        });

        //Bank menu
        actPayments = new QAction("Payments",window);
        actPayments->setStatusTip("Manage payments");
        mnuFunds->addAction(actPayments);
        connect(actPayments, &QAction::triggered, [this](){
            initNewPanel(Panel::PAYMENTS);
        });

        actDeposits = new QAction("Deposits",window);
        actDeposits->setStatusTip("Manage deposits");
        mnuFunds->addAction(actDeposits);

        //wire panels container control events
        connect(ui->tabMain, &QTabWidget::currentChanged, window, &MainWindow::ActivePanelChanged);
        connect(ui->tabMain, &QTabWidget::tabCloseRequested, window, &MainWindow::PanelCloseHandler);

        QList<PanelActions*> actions = PanelActions::LoadPanelActionDefinitions(window);
        for(PanelActions *action: actions)
        {
            panelActionRegistry.insert(action->getPanel(), action);
        }

        mnuAbout = window->menuBar()->addMenu("&About");
        actAbout = new QAction("About", window);
        actAbout->setStatusTip("About this program");
        mnuAbout->addAction(actAbout);

        connect(actAbout, &QAction::triggered, [this](){
            qDebug() << "About clicked";
        });
    }

    void loadSettings()
    {
        settingsManager = SettingsManager::initialize(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation),APP::ApplicationSettingsFile,APP::ApplicationConfigMetadataFile);
    }

    void initDatabase()
    {
        if(!settingsManager->initializeDatabaseConnection())
        {
            QMessageBox::critical(window, "Database Error", "Database could not be opened");
        }
    }

    PanelActions* getPanelActions(Panel panel)
    {
        if(panelActionRegistry.contains(panel))
        {
            return panelActionRegistry[panel];
        }
        return nullptr;
    }

    void initNewPanel(Panel panel)
    {
        bool exists = panel2Index.contains(panel);
        if(!exists || (exists && PanelFactory::PanelSupportsMultipleInstances(panel)))
        {
            PanelWindow *p = PanelFactory::CreateNewPanel(panel, window);
            int tabidx = ui->tabMain->addTab(p, p->panelName());

            //add panel to index
            if (panel2Index.contains(panel))
            {
                panel2Index[panel].append(tabidx);
            } else
            {
                panel2Index.insert(panel, QList<int>({tabidx}));
            }
            registerPanel(p, tabidx);
            ui->tabMain->setCurrentIndex(tabidx);
            activatePanel(p, tabidx);
        }
        else
        {
            if(exists)
            {
                //if a panel oif this type exists but a multiples are not allowed set focus to the first item
                //index in the panel registry
                ui->tabMain->setCurrentIndex(panel2Index[panel][0]);
            }
        }
    }

    void activatePanel(PanelWindow *panel, int index)
    {
        connect(panel, &PanelWindow::itemActionStateChange, window, &MainWindow::ContextItemStateChangeHandler);
        connect(window, &MainWindow::ContextItemActionTriggered, panel, &PanelWindow::itemActionHandler);
        PanelActions* actions = getPanelActions(panel->panelId());
        panel->activate(actions);
        actions->setVisibility(true);
    }

    void deactivatePanel(PanelWindow *panel, int index)
    {
        disconnect(panel, nullptr, window, nullptr);
        disconnect(window, nullptr, panel, nullptr);
        PanelActions* actions = getPanelActions(panel->panelId());
        panel->deactivate(actions);
        actions->setVisibility(false);
    }

    void unregisterPanel(PanelWindow *panel, int index)
    {
        if(panel == nullptr) return;
        index2Panel.remove(index);
        panel2Index.remove(panel->panelId());
    }

    void registerPanel(PanelWindow *panel, int index)
    {
        index2Panel[index]=panel;
    }

    void defaultState()
    {
        actSave->setEnabled(false);
        actEditItem->setEnabled(false);
        actAddItem->setEnabled(false);
        actDeleteItem->setEnabled(false);

        QList<QAction *> actions = mainToolbar->findChildren<QAction *>(QString(), Qt::FindDirectChildrenOnly);

        for(Panel p: panelActionRegistry.keys())
        {
            panelActionRegistry[p]->setVisibility(false);
        }
    }

private:
    MainWindow *window;
    Ui::MainWindow *ui;
    QToolBar *mainToolbar;
    QMenu *mnuFile;
    QMenu *mnuAbout;
    QMenu *mnuEdit;
    QMenu *mnuAccounts;
    QMenu *mnuFunds;
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
    QAction *actPayments;
    QAction *actDeposits;
    QAction *actFundsManagement;
    QMap<Panel, QList<int>> panel2Index;
    QMap<int, PanelWindow*> index2Panel;
    QMap<Panel,PanelActions*> panelActionRegistry;
    SettingsManager *settingsManager;
};

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), impl(std::shared_ptr<MainWindowImpl>(new MainWindowImpl(this)))
{
    impl->init();
}

MainWindow::~MainWindow()
{}

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


