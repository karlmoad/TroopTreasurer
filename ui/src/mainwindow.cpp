#include "ui/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setUnifiedTitleAndToolBarOnMac(true);
    init();
    loadSettings();
}

MainWindow::~MainWindow()
{
    SettingsManager::free();
    delete ui;
}

void MainWindow::init()
{
    mnuFile = this->menuBar()->addMenu("&File");
    mnuEdit = this->menuBar()->addMenu("&Edit");
    mnuAbout = this->menuBar()->addMenu("&About");
    defaultMenus.append({mnuFile, mnuEdit, mnuAbout});

    //File menu
    mnuFileOpenSubmenu = mnuFile->addMenu("Open");
    actFundsManagement = new QAction("Funds Management", this);
    mnuFileOpenSubmenu->addAction(actFundsManagement);
    actFundsManagement->setStatusTip("Open funds management panel");

    mainToolbar = this->addToolBar("MAIN");
    actSave = new QAction(QIcon(":/resources/disk.png"), "&Save", this);
    actSave->setStatusTip(tr("Save Item"));
    actSave->setEnabled(false);
    mainToolbar->addAction(actSave);
    mnuFile->addAction(actSave);
    connect(actSave, &QAction::triggered, [this](){
        emit ContextItemActionTriggered(ItemAction::SAVE);
    });
    defaultActions.append(actSave);


    mnuFileImportSubmenu = mnuFile->addMenu("Import");
    actImportEditTemplates = new QAction("Edit Templates", this);
    actImportEditTemplates->setStatusTip("Edit a data import template");
    connect(actImportEditTemplates, &QAction::triggered, [this](){
        OpenImportTemplateEditorPanel();
    });
    defaultActions.append(actImportEditTemplates);

    actImportData = new QAction("Import Data", this);
    actImportData->setStatusTip("Import a data file");
    mnuFileImportSubmenu->addAction(actImportEditTemplates);
    mnuFileImportSubmenu->addAction(actImportData);

    defaultActions.append(actImportData);

    actSettings = new QAction("Settings",this);
    mnuFile->addAction(actSettings);
    defaultActions.append(actSettings);
    connect(actSettings, &QAction::triggered, this, &MainWindow::SettingsHandler);

    mnuFile->addSeparator();
    actQuit = new QAction("&Quit", this);
    mnuFile->addAction(actQuit);

    defaultActions.append(actQuit);

    actAddItem = new QAction(QIcon(":/resources/page_white_add.png"),"Add item", this);
    actAddItem->setStatusTip("Add new item to current context");
    actAddItem->setEnabled(false);
    mnuEdit->addAction(actAddItem);
    mainToolbar->addAction(actAddItem);
    connect(actAddItem, &QAction::triggered, [this](){
        emit ContextItemActionTriggered(ItemAction::ADD);
    });
    defaultActions.append(actAddItem);

    actEditItem = new QAction(QIcon(":/resources/page_white_edit.png"),"Edit item", this);
    actEditItem->setStatusTip("Edit the current context selected item");
    actEditItem->setEnabled(false);
    mnuEdit->addAction(actEditItem);
    mainToolbar->addAction(actEditItem);
    connect(actEditItem, &QAction::triggered, [this](){
        emit ContextItemActionTriggered(ItemAction::EDIT);
    });
    defaultActions.append(actEditItem);

    actDeleteItem = new QAction(QIcon(":/resources/page_white_delete.png"),"Delete item",this);
    actDeleteItem->setStatusTip("Delete the current context selected item");
    actDeleteItem->setEnabled(false);
    mnuEdit->addAction(actDeleteItem);
    mainToolbar->addAction(actDeleteItem);
    connect(actDeleteItem, &QAction::triggered, [this](){
        emit ContextItemActionTriggered(ItemAction::DELETE);
    });
    defaultActions.append(actDeleteItem);

    actSep = mainToolbar->addSeparator();
    defaultActions.append(actSep);

    actAbout = new QAction("About", this);
    actAbout->setStatusTip("About this program");
    mnuAbout->addAction(actAbout);
    defaultActions.append(actAbout);

    connect(actAbout, &QAction::triggered, [this](){
        qDebug() << "About clicked";
    });

    //wire panels container control events
    connect(ui->tabMain, &QTabWidget::currentChanged, this, &MainWindow::ActivePanelChanged);
    connect(ui->tabMain, &QTabWidget::tabCloseRequested, this, &MainWindow::PanelCloseHandler);
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
    actSave->setEnabled(state.saveEnabled());
    actAddItem->setEnabled(state.addEnabled());
    actEditItem->setEnabled(state.editEnabled());
    actDeleteItem->setEnabled(state.deleteEnabled());
}

void MainWindow::PanelCloseHandler(int panelIdx)
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

void MainWindow::OpenImportTemplateEditorPanel()
{
    initNewPanel(Panel::IMPORT_TEMPLATE_EDITOR);
}

QMenu *MainWindow::getMenu(const QString &text)
{
    QList<QMenu*> r =  this->menuBar()->findChildren<QMenu*>(QString(), Qt::FindDirectChildrenOnly);
    for(int i=0; i<r.count(); i++)
    {
        if(r[i]->title().compare(text, Qt::CaseSensitive) ==0)
        {
            return r[i];
        }
    }
    return nullptr;
}

void MainWindow::initNewPanel(Panel panel)
{
    PanelWindow *p = PanelFactory::CreateNewPanel(panel,this);
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

void MainWindow::unregisterPanel(PanelWindow *panel, int index)
{
    if(panel == nullptr) return;

    if(panel->hasMenu())
    {
        QMenu *menu = getMenu(panel->panelMenuText());
        if(menu != nullptr)
        {
            if (panelMenus.contains(index))
            {
                for (int i = 0; i < panelMenus[index].count(); i++)
                {
                    menu->removeAction(panelMenus[index][i]);
                }
                panelMenus.remove(index);
            }
        }
    }

    if(panel->hasToolbarItems())
    {
        if (panelToolbarItems.contains(index))
        {
            for (int i = 0; i < panelToolbarItems[index].count(); i++)
            {
                mainToolbar->removeAction(panelToolbarItems[index][i]);
            }
            panelToolbarItems.remove(index);
        }
    }

    index2Panel.remove(index);
}

void MainWindow::registerPanel(PanelWindow *panel, int index)
{
    index2Panel[index]=panel;
    if(panel->hasMenu())
    {
        QMenu *menu = getMenu(panel->panelMenuText());
        if(menu == nullptr)
        {
            menu = menuBar()->addMenu(panel->panelMenuText());
        }

        QList<QAction*> actions = panel->menuItems();
        for(int i=0;i<actions.count(); i++)
        {
            menu->addAction(actions[i]);
        }
        panelMenus[index] = actions;
    }

    if(panel->hasToolbarItems())
    {
        QList<QAction*> actions = panel->toolbarItems();
        for(int i=0;i<actions.count(); i++)
        {
            mainToolbar->addAction(actions[i]);
        }
        panelToolbarItems[index] = actions;
    }
}

void MainWindow::activatePanel(PanelWindow *panel, int index)
{
    connect(panel, &PanelWindow::itemActionStateChange, this, &MainWindow::ContextItemStateChangeHandler);
    connect(this, &MainWindow::ContextItemActionTriggered, panel, &PanelWindow::itemActionHandler);
    setPanelMenuToolbarVisibility(panel,index,true);
    panel->activate();
}

void MainWindow::deactivatePanel(PanelWindow *panel, int index)
{
    disconnect(panel, nullptr, this, nullptr);
    disconnect(this, nullptr, panel, nullptr);
    setPanelMenuToolbarVisibility(panel,index,false);
}

void MainWindow::setPanelMenuToolbarVisibility(PanelWindow *panel, int index, bool visible)
{
    if(panel)
    {

        if (panel->hasMenu())
        {
            QMenu *menu = getMenu(panel->panelMenuText());
            if (menu != nullptr && panelMenus.contains(index))
            {
                for (int i = 0; i < panelMenus[index].count(); i++)
                {
                    panelMenus[index][i]->setVisible(visible);
                }
            }

            if (visible)
            {
                menu->setHidden(false);
            } else
            {
                //hide the menu iof all actions are hidden

                QList<QAction *> items = menu->findChildren<QAction *>();
                bool show = false;
                for (int i = 0; i < items.count(); i++)
                {
                    if (!items[i]->isVisible())
                    {
                        show = true;
                        break;
                    }
                }

                if (!show)
                {
                    menu->setHidden(true);
                }
            }
        }

        if (panel->hasToolbarItems())
        {
            if (panelToolbarItems.contains(index))
            {
                for (int i = 0; i < panelToolbarItems[index].count(); i++)
                {
                    panelToolbarItems[index][i]->setVisible(visible);
                }
            }
        }
    }
}

void MainWindow::ActivePanelChanged(int index)
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

void MainWindow::defaultState()
{
    actSave->setEnabled(false);
    actEditItem->setEnabled(false);
    actAddItem->setEnabled(false);
    actDeleteItem->setEnabled(false);

    QList<QAction *> actions = mainToolbar->findChildren<QAction*>(QString(), Qt::FindDirectChildrenOnly);
    for(int i =0; i< actions.count(); i++)
    {
        if(!defaultActions.contains(actions[i]))
        {
            mainToolbar->removeAction(actions[i]);
        }
    }

    this->menuBar()->clear();
    for(int i=0; i<defaultMenus.count(); i++)
    {
        this->menuBar()->addMenu(defaultMenus[i]);
    }
}

void MainWindow::SettingsHandler()
{
    ApplicationSettingsDialog *appSettings = new ApplicationSettingsDialog(this);
    appSettings->setModal(true);
    appSettings->show();
}

void MainWindow::loadSettings()
{
    settingsManager = SettingsManager::initialize(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation),APP::ApplicationSettingsFile,APP::ApplicationConfigMetadataFile);
}
