#include "fundsregisterpanel.h"
#include "ui_fundsregisterpanel.h"

FundsRegisterPanel::FundsRegisterPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FundsRegisterPanel)
{
    ui->setupUi(this);
}

FundsRegisterPanel::~FundsRegisterPanel()
{
    delete ui;
}
