#include <ui/fundsrecordeditdialog.h>
#include "ui_fundsrecordeditdialog.h"
#include <QMessageBox>

class FundsRecordEditDialog::FundsRecordEditDialogImpl
{
public:
    FundsRecordEditDialogImpl(FundsRecordEditDialog *dialog): _dialog(dialog), _ui(new Ui::FundsRecordEditDialog)
    {
        _ui->setupUi(_dialog);
        connect(_ui->btnOk, &QPushButton::clicked, _dialog, &FundsRecordEditDialog::okHandler);
        connect(_ui->btnCancel, &QPushButton::clicked, _dialog, &FundsRecordEditDialog::cancelHandler);
        init();
    }

    void init()
    {
        QMap<Transactions::TransactionTypes::Type, QString> types = Transactions::TransactionTypes::map();
        for(Transactions::TransactionTypes::Type type : types.keys())
        {
            _ui->ctrlMethod->addItem(types[type], Transactions::TransactionTypes::typeToInt(type));
        }
        _ui->ctrlMethod->setCurrentIndex(0);
    }

    void setRecord(Transactions::FundsRecord *record, UI::Action action)
    {
        _action = action;
        _record = record;
        loadRecord();
    }

    void loadRecord()
    {
        if(_record && _action == UI::Action::EDIT)
        {
            for(int i = 0; i<_ui->ctrlMethod->count(); i++)
            {
                if(static_cast<int>(_record->method()) == _ui->ctrlMethod->itemData(i).toInt())
                {
                    _ui->ctrlMethod->setCurrentIndex(1);
                    break;
                }
            }
        }
        _ui->ctrlDate->setDate((_action == UI::Action::EDIT ? _record->date() : QDate::currentDate()));
        _ui->ctrlAmount->setValue((_action == UI::Action::EDIT ? _record->amount() : 0.0));
        _ui->ctrlRef->setText((_action == UI::Action::EDIT ? _record->referenceValue() : ""));
        _ui->ctrlWho->setText((_action == UI::Action::EDIT ? _record->who() : ""));
        _ui->ctrlWhat->setText((_action == UI::Action::EDIT ? _record->what() : ""));
        _ui->txtComments->setPlainText((_action == UI::Action::EDIT ? _record->comments() : ""));
        _ui->optReconciled->setCheckState((_action == UI::Action::EDIT && _record->reconciled() ? Qt::Checked : Qt::Unchecked));
    }

    bool validateAndSync()
    {
        bool valid = true;
        QString msg = "Issues exist with the following fields:\n";

        if(_ui->ctrlMethod->currentIndex() == 0)
        {
            msg.append("Select a valid method.\n");
            valid = false;
        }

        if(_ui->ctrlAmount->value() <= 0)
        {
            msg.append("Amount must be greater than 0.\n");
            valid = false;
        }

        if(_record && valid)
        {
            _record->setDate(_ui->ctrlDate->date());
            _record->setMethod(Transactions::TransactionTypes::intToType(_ui->ctrlMethod->currentData().toInt()));
            _record->setAmount(_ui->ctrlAmount->value());
            _record->setReferenceValue(_ui->ctrlRef->text());
            _record->setWho(_ui->ctrlWho->text());
            _record->setWhat(_ui->ctrlWhat->text());
            _record->setComments(_ui->txtComments->toPlainText());
            _record->setReconciled(_ui->optReconciled->isChecked());
        }
        else
        {
            QMessageBox::critical(_dialog, "Errors", msg);
        }
        return valid;
    }


private:
    Ui::FundsRecordEditDialog *_ui;
    FundsRecordEditDialog *_dialog;
    UI::Action _action;
    Transactions::FundsRecord *_record;
};

FundsRecordEditDialog::FundsRecordEditDialog(QWidget *parent) :
    QDialog(parent),
    impl(new FundsRecordEditDialogImpl(this)){}

FundsRecordEditDialog::~FundsRecordEditDialog(){}

void FundsRecordEditDialog::setRecord(Transactions::FundsRecord *record, UI::Action action)
{
    impl->setRecord(record,action);
}

void FundsRecordEditDialog::okHandler()
{
    if(impl->validateAndSync())
    {
        this->done(QDialog::Accepted);
        this->close();
    }
}

void FundsRecordEditDialog::cancelHandler()
{
    this->done(QDialog::Rejected);
    this->close();
}
