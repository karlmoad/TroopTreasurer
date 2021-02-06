#include "ui/accounteditdialog.h"
#include "ui/accountpickerdialog.h"
#include "ui_accounteditdialog.h"
#include <QMessageBox>
#include <QDebug>

class AccountEditDialog::AccountEditDialogImpl
{
public:
    explicit AccountEditDialogImpl(AccountEditDialog *dialog, AccountsModel *model, ItemAction action): _dialog(dialog),
                                                                                                    _ui(new Ui::AccountEditDialog),
                                                                                                    _model(model),
                                                                                                    _action(action),
                                                                                                    _account(nullptr)
    {
        _ui->setupUi(_dialog);
        connect(_ui->btnOk, &QPushButton::clicked, _dialog, &AccountEditDialog::okHandler);
        connect(_ui->btnCancel, &QPushButton::clicked, _dialog, &AccountEditDialog::cancelHandler);
        connect(_ui->btnParent, &QPushButton::clicked, _dialog, &AccountEditDialog::parentSelectHandler);
        connect(_ui->btnReset, &QPushButton::clicked, _dialog, &AccountEditDialog::parentResetHandler);
    }

    ~AccountEditDialogImpl()
    {
        delete _ui;
    }

    void setContext(const QModelIndex& index)
    {
        if(_action == ItemAction::EDIT)
        {
            _context = index;
            _account = _model->getAccount(index);
            load();
        }
    }

    void resetParent()
    {
        switch(_action)
        {
            case ItemAction::EDIT:
            {
                _newParentSet = true;
                break;
            }
            default:
                break;
        }
        _newParent = QModelIndex();
        _ui->txtParent->setText("");
    }

    void load()
    {
        _ui->txtName->setText(_account->name());
        _ui->txtSourceID->setText(_account->sourceKey());
        _ui->txtOrg->setText(_account->org());
        _ui->chkReported->setCheckState((_account->isReported() ? Qt::Checked : Qt::Unchecked));
        _ui->chkRollup->setCheckState((_account->isRollup() ? Qt::Checked : Qt::Unchecked));
        _ui->chkExtrenal->setCheckState((_account->isExternal() ? Qt::Checked : Qt::Unchecked));
        _ui->chkClosed->setCheckState((_account->isClosed() ? Qt::Checked : Qt::Unchecked));

        std::shared_ptr<Account> parent = _model->getAccount(_context.parent());
        if(parent)
            _ui->txtParent->setText(parent->name());

    }

    bool confirmRootParentLinkage()
    {
        auto r = QMessageBox::question(_dialog,
                                       "No Parent Account",
                                       "You have not selected a parent account, if you continue this account will be linked at the root level.\n\nYES to continue, NO to stop.",
                                       QMessageBox::Yes | QMessageBox::No);
        return r == QMessageBox::Yes;
    }

    bool save()
    {
        if(_action == ItemAction::ADD)
        {
            _account = std::shared_ptr<Account>(new Account());
        }

        _account->setOrg(_ui->txtOrg->text());
        _account->setName(_ui->txtName->text());
        _account->setSourceKey(_ui->txtSourceID->text());
        _account->setClosed(_ui->chkClosed->isChecked());
        _account->setExternal(_ui->chkExtrenal->isChecked());
        _account->setRollup(_ui->chkRollup->isChecked());
        _account->setReported(_ui->chkReported->isChecked());

        switch(_action)
        {
            case ItemAction::EDIT:
            {
                _model->updateAccount(_context);
                // handle parent change
                if(_newParentSet && _newParent != _context.parent())
                {
                    bool pass = true;
                    if(!_newParent.isValid()) pass = confirmRootParentLinkage();
                    if(pass)
                    {
                        _model->moveAccount(_context, _newParent);
                        return true;
                    }
                    return false;
                }
                return true;
            }
            case ItemAction::ADD:
            {
                bool pass = true;
                if (!_newParent.isValid()) pass = confirmRootParentLinkage();
                if (pass)
                {
                    _model->addAccount(*(_account.get()), _newParent);
                    return true;
                }
                return false;
            }
            default:
                return false;
        }
    }

    void parentSelect()
    {
        AccountPickerDialog *picker = new AccountPickerDialog(_model);
        if(_action == ItemAction::EDIT)
            picker->setContextItem(_context);
        picker->setModal(true);
        int r = picker->exec();
        if(r == QDialog::Accepted)
        {
            _newParentSet = true;
            _newParent =  picker->getSelected();
            auto newParent = _model->getAccount(_newParent);
            if(newParent)
            {
                _ui->txtParent->setText(newParent->name());
            }
            else
            {
                _newParentSet = false;
                _newParent = QModelIndex();
            }
        }
        delete picker;
    }

private:
    AccountEditDialog *_dialog;
    AccountsModel *_model;
    Ui::AccountEditDialog *_ui;
    ItemAction _action;
    QModelIndex _context;
    std::shared_ptr<Account> _account;
    bool _newParentSet;
    QModelIndex _newParent;
};

AccountEditDialog::AccountEditDialog(AccountsModel* model, ItemAction action, QWidget *parent) :
    QDialog(parent),
    impl(new AccountEditDialogImpl(this, model, action))
{

}

AccountEditDialog::~AccountEditDialog(){}

void AccountEditDialog::setContextItem(const QModelIndex &index)
{
    impl->setContext(index);
}

void AccountEditDialog::okHandler()
{
    if(impl->save())
    {
        this->done(QDialog::Accepted);
        this->close();
    }
}

void AccountEditDialog::cancelHandler()
{
    this->done(QDialog::Rejected);
    this->close();
}

void AccountEditDialog::parentSelectHandler()
{
    impl->parentSelect();
}

void AccountEditDialog::parentResetHandler()
{
    impl->resetParent();
}
