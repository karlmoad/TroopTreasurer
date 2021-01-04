//
// Created by Karl Moad on 12/28/20.
//

// You may need to build the project (run Qt uic code generator) to get "ui_importtemplatetestdialog.h" resolved

#include "ui/importtemplatetestdialog.h"
#include "ui_importtemplatetestdialog.h"

importtemplatetestdialog::importtemplatetestdialog(QWidget *parent) :
        QWidget(parent), ui(new Ui::importtemplatetestdialog)
{
    ui->setupUi(this);
}

importtemplatetestdialog::~importtemplatetestdialog()
{
    delete ui;
}
