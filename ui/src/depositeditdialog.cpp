#include "depositeditdialog.h"
#include "ui_depositeditdialog.h"

DepositEditDialog::DepositEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DepositEditDialog)
{
    ui->setupUi(this);
}

DepositEditDialog::~DepositEditDialog()
{
    delete ui;
}
