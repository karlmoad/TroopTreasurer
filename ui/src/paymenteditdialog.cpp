#include "ui/paymenteditdialog.h"
#include "ui_paymenteditdialog.h"
#include <QMessageBox>

class PaymentEditDialog::PaymentEditDialogImpl
{
public:
    PaymentEditDialogImpl(PaymentEditDialog *dialog): _dialog(dialog), _ui(new Ui::PaymentEditDialog)
    {
        _ui->setupUi(_dialog);
        connect(_ui->btnOk, &QPushButton::clicked, _dialog, &PaymentEditDialog::okHandler);
        connect(_ui->btnCancel, &QPushButton::clicked, _dialog, &PaymentEditDialog::cancelHandler);
        init();
    }

    ~PaymentEditDialogImpl()
    {
        delete _ui;
    }

    void init()
    {
        QMap<Transactions::TransactionTypes::Type, QString> types = Transactions::TransactionTypes::map();
        int i=0, s=0;
        for(Transactions::TransactionTypes::Type type : types.keys())
        {
            _ui->ctrlMethod->addItem(types[type], Transactions::TransactionTypes::typeToInt(type));
            if(_record && _action == UI::Action::EDIT)
            {
                if(_record->method() == type)
                {
                    s = i;
                    break;
                }
            }
            i++;
        }
        _ui->ctrlMethod->setCurrentIndex(s);

        _ui->ctrlDate->setDate((_action == UI::Action::EDIT ? _record->date() : QDate::currentDate()));
        _ui->ctrlAmount->setValue((_action == UI::Action::EDIT ? _record->amount() : 0.0));
        _ui->ctrlRef->setText((_action == UI::Action::EDIT ? _record->referenceValue() : ""));
        _ui->ctrlWho->setText((_action == UI::Action::EDIT ? _record->who() : ""));
        _ui->ctrlWhat->setText((_action == UI::Action::EDIT ? _record->what() : ""));
        _ui->txtComments->setPlainText((_action == UI::Action::EDIT ? _record->comments() : ""));
        _ui->optReconciled->setCheckState((_action == UI::Action::EDIT && _record->reconciled() ? Qt::Checked : Qt::Unchecked));
        _ui->optFinalized->setCheckState((_action == UI::Action::EDIT && _record->finalized() ? Qt::Checked : Qt::Unchecked));
    }

    void setAction(UI::Action action)
    {
        _action = action;
    }

    void setRecord(Transactions::Payment *record)
    {
        _record = record;
    }

    bool validateAndSync()
    {
        bool valid = true;
        QString msg = "Issues exist with the following fields:\n";

         if(_ui->ctrlMethod->currentIndex() == 0)
         {
             msg.append("Select a valid payment method.\n");
             valid = false;
         }

         if(_ui->ctrlAmount->value() == 0)
         {
            msg.append("Payment amount must be greater than 0.\n");
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
            _record->setReconciled(_ui->optReconciled->isChecked());
            _record->setFinalized(_ui->optFinalized->isChecked());
        }
        else
        {
            QMessageBox::critical(_dialog, "Errors", msg);
        }
        return valid;
    }

private:
    Ui::PaymentEditDialog *_ui;
    PaymentEditDialog *_dialog;
    UI::Action _action;
    Transactions::Payment *_record;
};

PaymentEditDialog::PaymentEditDialog(QWidget *parent) :
    QDialog(parent),
    impl(new PaymentEditDialogImpl(this))
{}

PaymentEditDialog::~PaymentEditDialog()
{}

void PaymentEditDialog::setRecord(Transactions::Payment *payment)
{
    impl->setRecord(payment);
}

void PaymentEditDialog::setAction(UI::Action action)
{
    impl->setAction(action);
}

void PaymentEditDialog::okHandler()
{
    if(impl->validateAndSync())
    {
        this->done(QDialog::Accepted);
        this->close();
    }
}

void PaymentEditDialog::cancelHandler()
{
    this->done(QDialog::Rejected);
    this->close();
}
