#include "ui/depositeditpicklistdialog.h"
#include "ui_depositeditpicklistdialog.h"
#include "objects/fundsrecord.h"
#include "objects/fundsrecordsmodel.h"

class DepositEditPicklistDialog::DepositEditPicklistDialogImpl
{
public:
    DepositEditPicklistDialogImpl(DepositEditPicklistDialog *dialog): _dialog(dialog), _ui(new Ui::DepositEditPicklistDialog)
    {
        _ui->setupUi(_dialog);
        _model = new Transactions::FundsRecordsModel(_dialog);
        _ui->tableFunds->setModel(_model);

        _model->load();
        connect(_ui->btnOk, &QPushButton::clicked, _dialog, &DepositEditPicklistDialog::okHandler);
        connect(_ui->btnCancel, &QPushButton::clicked, _dialog, &DepositEditPicklistDialog::cancelHandler);
    }

    bool setSelection()
    {
        _selected = _ui->tableFunds->selectionModel()->selectedRows();
        if(_selected.count() > 0)
        {
            return true;
        }
        return false;
    }

    QList<Transactions::FundsRecord> getSelected()
    {
        QList<Transactions::FundsRecord> out;
        for(QModelIndex idx: _selected)
        {
            out.append(*(_model->getRecord(idx).get()));
        }
        return out;
    }

private:
    DepositEditPicklistDialog *_dialog;
    Ui::DepositEditPicklistDialog *_ui;
    Transactions::FundsRecordsModel *_model;
    QList<QModelIndex> _selected;
};

DepositEditPicklistDialog::DepositEditPicklistDialog(QWidget *parent) :
    QDialog(parent),
    impl(new DepositEditPicklistDialogImpl(this)){}

DepositEditPicklistDialog::~DepositEditPicklistDialog(){}

QList<Transactions::FundsRecord> DepositEditPicklistDialog::getSelected()
{
    return QList<Transactions::FundsRecord>();
}

void DepositEditPicklistDialog::okHandler()
{
    if(impl->setSelection())
    {
        this->done(QDialog::Accepted);
        this->close();
    }
}

void DepositEditPicklistDialog::cancelHandler()
{
    this->done(QDialog::Rejected);
    this->close();
}
