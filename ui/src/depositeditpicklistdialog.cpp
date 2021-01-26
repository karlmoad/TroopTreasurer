#include "ui/depositeditpicklistdialog.h"
#include "ui_depositeditpicklistdialog.h"

class DepositEditPicklistDialog::DepositEditPicklistDialogImpl
{
public:
    DepositEditPicklistDialogImpl(DepositEditPicklistDialog *dialog): _dialog(dialog), _ui(new Ui::DepositEditPicklistDialog)
    {
        _ui->setupUi(_dialog);
    }

private:
    DepositEditPicklistDialog *_dialog;
    Ui::DepositEditPicklistDialog *_ui;
};

DepositEditPicklistDialog::DepositEditPicklistDialog(QWidget *parent) :
    QDialog(parent),
    impl(new DepositEditPicklistDialogImpl(this)){}

DepositEditPicklistDialog::~DepositEditPicklistDialog(){}
