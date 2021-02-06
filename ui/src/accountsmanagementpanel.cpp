#include "ui/accountsmanagementpanel.h"
#include "ui_accountsmanagementpanel.h"

accountsmanagementpanel::accountsmanagementpanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::accountsmanagementpanel)
{
    ui->setupUi(this);
}

accountsmanagementpanel::~accountsmanagementpanel()
{
    delete ui;
}
