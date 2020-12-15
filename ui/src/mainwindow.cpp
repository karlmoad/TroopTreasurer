#include "ui/mainwindow.h"
#include "ui_mainwindow.h"


struct MainWindow::MainWindowImpl
{
    Ui::MainWindow *ui;
    //QMenu *mnuFile;
    QToolBar *primary_toolbar;
    QAction *actSave;

};

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), impl(new MainWindowImpl())
{
    impl->ui = new Ui::MainWindow();
    impl->ui->setupUi(this);

    impl->primary_toolbar = this->addToolBar("File");

    impl->actSave = new QAction(QIcon(":/resources/disk.png"), tr("&SaVE"), this);
    impl->actSave->setStatusTip(tr("Save Item"));
    impl->primary_toolbar->addAction(impl->actSave);


}

MainWindow::~MainWindow()
{
    delete impl;
}
