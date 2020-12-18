#include "ui/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setUnifiedTitleAndToolBarOnMac(true);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    mnuFile = this->menuBar()->addMenu("&File");
    mnuEdit = this->menuBar()->addMenu("&Edit");
    mnuAbout = this->menuBar()->addMenu("&About");

    //File menu
    mnuFileOpenSubmenu = mnuFile->addMenu("Open");
    actFundsManagement = new QAction("Funds Management", this);
    mnuFileOpenSubmenu->addAction(actFundsManagement);
    actFundsManagement->setStatusTip("Open funds management panel");

    mainToolbar = this->addToolBar("MAIN");
    actSave = new QAction(QIcon(":/resources/disk.png"), "&Save", this);
    actSave->setStatusTip(tr("Save Item"));
    mainToolbar->addAction(actSave);
    mnuFile->addAction(actSave);
    connect(actSave, &QAction::triggered, [this](){
        emit SaveItemInContext();
    });

    mnuFileImportSubmenu = mnuFile->addMenu("Import");
    actImportEditTemplates = new QAction("Edit Templates", this);
    actImportEditTemplates->setStatusTip("Edit a data import template");

    actImportData = new QAction("Import Data", this);
    actImportData->setStatusTip("Import a data file");

    mnuFileImportSubmenu->addAction(actImportEditTemplates);
    mnuFileImportSubmenu->addAction(actImportData);

    mnuFile->addSeparator();
    actQuit = new QAction("&Quit", this);
    mnuFile->addAction(actQuit);

    actAddItem = new QAction("Add new item", this);
    actAddItem->setStatusTip("Add new item to current context");
    mnuEdit->addAction(actAddItem);
    connect(actAddItem, &QAction::triggered, [this](){
        emit NewItemInContext();
    });

    actEditItem = new QAction("Edit item", this);
    actEditItem->setStatusTip("Edit the current context selected item");
    mnuEdit->addAction(actEditItem);
    connect(actEditItem, &QAction::triggered, [this](){
        emit EditItemInContext();
    });

    actDeleteItem = new QAction("Delete item",this);
    actDeleteItem->setStatusTip("Delete the current context selected item");
    connect(actDeleteItem, &QAction::triggered, [this](){
        emit DeleteItemInContext();
    });

    actAbout = new QAction("About", this);
    actAbout->setStatusTip("About this program");
    mnuAbout->addAction(actAbout);

    connect(actAbout, &QAction::triggered, [this](){
        qDebug() << "About clicked";
    });
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

void MainWindow::NotifyContextItemStateChange(const ItemState &state)
{

}

void MainWindow::PanelCloseHandler(int panel)
{

}

void MainWindow::OpenFundsManagementPanel()
{

}

void MainWindow::ConnectPanel(QWidget *panel, const QString &header)
{

}


