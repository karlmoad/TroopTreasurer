#include "ui/depositeditdialog.h"
#include "ui_depositeditdialog.h"
#include "objects/depositfundsmodel.h"
#include "ui/depositeditpicklistdialog.h"
#include <QMessageBox>

class DepositEditDialog::DepositEditDialogImpl
{
public:
    DepositEditDialogImpl(DepositEditDialog *dialog): _dialog(dialog), _ui(new Ui::DepositEditDialog)
    {
        _ui->setupUi(_dialog);
        connect(_ui->btnOk, &QPushButton::clicked, _dialog, &DepositEditDialog::okHandler);
        connect(_ui->btnCancel, &QPushButton::clicked, _dialog, &DepositEditDialog::cancelHandler);
        connect(_ui->btnAdd, &QPushButton::clicked, _dialog, &DepositEditDialog::addHandler);
        connect(_ui->btnRemove, &QPushButton::clicked, _dialog, &DepositEditDialog::remHandler);
        _model = new Transactions::DepositFundsModel(_dialog);
        _ui->tableFunds->setModel(_model);

        //remove cancel button, all actions performed are direct database transactions
        //thus canceling of the form implies rejection of actions, but this needs to be at the item level.
        //hidden via code in place of deletion to provide future modification of this process.
        _ui->btnCancel->setEnabled(false);
        _ui->btnCancel->setHidden(true);
        //------

    }

    void init(UI::Action action, std::shared_ptr<Transactions::Deposit> deposit)
    {
        _deposit = deposit;
        if(action == UI::Action::NONE)
        {
            _ui->btnAdd->setEnabled(false);
            _ui->btnAdd->setHidden(true);
            _ui->btnRemove->setEnabled(false);
            _ui->btnRemove->setHidden(true);
        }

        _ui->dateDeposit->setDate(_deposit->date());
        syncTotalDisplay();
        _model->setDeposit(_deposit);
    }

    void syncTotalDisplay()
    {
        _ui->lcdTotal->setMode(QLCDNumber::Dec);
        _ui->lcdTotal->display(_deposit->total());
    }


    void sync()
    {
        _deposit->setDate(_ui->dateDeposit->date());
    }

    void add()
    {
        DepositEditPicklistDialog *picker = new DepositEditPicklistDialog(_dialog);
        int r = picker->exec();
        if(r == QDialog::Accepted)
        {
            QList<Transactions::FundsRecord> items = picker->getSelected();
            for(int i=0; i<items.length(); i++)
            {
                _model->addFundsRecord(Transactions::FundsRecord(items[i]));
            }
        }
        _deposit->setTotal(_model->sumTotal());
        syncTotalDisplay();
    }

    void remove()
    {
        QModelIndexList selected = _ui->tableFunds->selectionModel()->selectedRows();
        if(selected.count() ==1)
        {
            _model->removeFundsRecord(selected[0]);
        }
        else
        {
            QMessageBox::critical(_dialog, "Error", "Remove operation can only be performed one row at a time");
        }
        _deposit->setTotal(_model->sumTotal());
        syncTotalDisplay();
    }

private:
    DepositEditDialog *_dialog;
    Ui::DepositEditDialog *_ui;
    Transactions::DepositFundsModel *_model;
    std::shared_ptr<Transactions::Deposit> _deposit;
};

DepositEditDialog::DepositEditDialog(QWidget *parent) :
    QDialog(parent),
    impl(new DepositEditDialogImpl(this)){}

DepositEditDialog::~DepositEditDialog(){}

void DepositEditDialog::setDeposit(std::shared_ptr<Transactions::Deposit> deposit, UI::Action action)
{

}

void DepositEditDialog::okHandler()
{
    this->done(QDialog::Accepted);
    this->close();
}

void DepositEditDialog::cancelHandler()
{
    //do nothing see comments concerning hiding the cancel functionality
}

void DepositEditDialog::addHandler()
{
    impl->add();
}

void DepositEditDialog::remHandler()
{
    impl->remove();
}
