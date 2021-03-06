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
#include <QMessageBox>
#include "ui/panelfactory.h"
#include "ui/panelwindow.h"
#include "ui/panelactions.h"
#include "ui/applicationsettingsdialog.h"
#include "ui/applicationconstants.h"
#include "ui/importdatadialog.h"
#include "objects/settingsmanager.h"
#include "objects/databasemanager.h"
#include "objects/objecterror.h"

class MainWindow::MainWindowImpl
{
public:
    MainWindowImpl(MainWindow *w): window(w), ui(new Ui::MainWindow)
    {
        ui->setupUi(window);
        window->setUnifiedTitleAndToolBarOnMac(true);
        databaseMgr = std::shared_ptr<DatabaseManager>(new DatabaseManager(":/resources/files/database.json"));
        loadSettings();
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
        connect2DB();
    }

    void connect2DB()
    {
        bool enable = true;
        QString msg;
        if(!initDatabase(msg))
        {
            enable=false;
            QMessageBox::critical(window, "Database Error", "Database error,\n" + msg);
        }
        enableMainFunctionItems(enable);
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
    void enableMainFunctionItems(bool enable)
    {
        actImportEditTemplates->setEnabled(enable);
        actImportData->setEnabled(enable);
        actOpenAccounts->setEnabled(enable);
        actAccountBalancesReport->setEnabled(enable);
        actNegBalReport->setEnabled(enable);
        actFundsRegister->setEnabled(enable);
        actPayments->setEnabled(enable);
        actDeposits->setEnabled(enable);
        actConnectDb->setEnabled(!enable);
    }

    void initMenuAndToolbar()
    {
        mnuFile = window->menuBar()->addMenu("&File");
        mnuEdit = window->menuBar()->addMenu("&Edit");
        mnuAccounts = window->menuBar()->addMenu("Accounts");
        mnuFunds = window->menuBar()->addMenu("Bank");

        //File menu

        actConnectDb = new QAction(QIcon(":/resources/database_connect.png"), "Connect Database", this->window);
        actConnectDb->setStatusTip("Connect to the application database");
        mnuFile->addAction(actConnectDb);
        connect(actConnectDb, &QAction::triggered, window, &MainWindow::ConnectHandler);

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

        //Accounts menu
        actOpenAccounts = new QAction("Account Management", window);
        actOpenAccounts->setStatusTip("Open Account Management");
        mnuAccounts->addAction(actOpenAccounts);
        connect(actOpenAccounts, &QAction::triggered, [this](){
            initNewPanel(Panel::ACCOUNTSMGMT);
        });

        mnuReports = mnuAccounts->addMenu("Reports");

        actAccountBalancesReport = new QAction("Account Balances Report",window);
        actAccountBalancesReport->setStatusTip("View Account Balances Report");
        mnuReports->addAction(actAccountBalancesReport);
        connect(actAccountBalancesReport, &QAction::triggered, [this](){
            initNewPanel(Panel::ACCTBALREPORT);
        });

        actNegBalReport = new QAction("Negative Balance Report", window);
        actNegBalReport->setStatusTip("View Negative Balance report");
        mnuReports->addAction(actNegBalReport);
        connect(actNegBalReport, &QAction::triggered, [this](){
            initNewPanel(Panel::NEGBALREPORT);
        });

        //Bank menu
        actFundsRegister = new QAction("Funds Register", window);
        actFundsRegister->setStatusTip("Manage Funds Register");
        mnuFunds->addAction(actFundsRegister);
        connect(actFundsRegister, &QAction::triggered, [this](){
            initNewPanel(Panel::FUNDS);
        });

        actPayments = new QAction("Payments",window);
        actPayments->setStatusTip("Manage payments");
        mnuFunds->addAction(actPayments);
        connect(actPayments, &QAction::triggered, [this](){
            initNewPanel(Panel::PAYMENTS);
        });

        actDeposits = new QAction("Deposits",window);
        actDeposits->setStatusTip("Manage deposits");
        mnuFunds->addAction(actDeposits);
        connect(actDeposits, &QAction::triggered, [this](){
            initNewPanel(Panel::DEPOSITS);
        });

        //wire panels container control events
        connect(ui->tabMain, &QTabWidget::currentChanged, window, &MainWindow::ActivePanelChanged);
        connect(ui->tabMain, &QTabWidget::tabCloseRequested, window, &MainWindow::PanelCloseHandler);


        QList<PanelActions *> actions = PanelActions::LoadPanelActionDefinitions(window);
        for (PanelActions *action: actions)
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

    bool initDatabase(QString& message)
    {
        if(databaseMgr)
        {
            DatabaseValidationResponse r = databaseMgr->createDatabaseConnection();
            switch(r)
            {
                case DatabaseValidationResponse::OPEN:
                {
                    return true;
                }

                case DatabaseValidationResponse::SCHEMA_NOT_INITIALIZED:
                {
                    QJsonObject results = databaseMgr->initializeDatabaseObjects();
                    bool success = true;
                    QString msg = "Failed to initialize the following database objects:\n";
                    for (QString key : results.keys())
                    {
                        if (!results[key].toBool())
                        {
                            success = false;
                            msg.append(QString("%1\n").arg(key));
                        }
                    }

                    if (!success)
                        message = msg;

                    return success;
                }
                default:
                {
                    editSettings();
                    return false;
                }
            }
        }
        return false;
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
    QMenu *mnuReports;
    QAction *actConnectDb;
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
    QAction *actFundsRegister;
    QAction *actAccountBalancesReport;
    QAction *actNegBalReport;
    QMap<Panel, QList<int>> panel2Index;
    QMap<int, PanelWindow*> index2Panel;
    QMap<Panel,PanelActions*> panelActionRegistry;
    SettingsManager *settingsManager;
    std::shared_ptr<DatabaseManager> databaseMgr;
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

void MainWindow::ConnectHandler()
{
    impl->connect2DB();
}


