#include "despoiteditpicklistdialog.h"
#include "ui_despoiteditpicklistdialog.h"

DespoitEditPicklistDialog::DespoitEditPicklistDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DespoitEditPicklistDialog)
{
    ui->setupUi(this);
}

DespoitEditPicklistDialog::~DespoitEditPicklistDialog()
{
    delete ui;
}
