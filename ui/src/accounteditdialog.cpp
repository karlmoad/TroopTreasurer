#include "ui/accounteditdialog.h"
#include "ui/accountpickerdialog.h"
#include "ui_accounteditdialog.h"
#include <QMessageBox>
#include <QDebug>
#include "objects/objecterror.h"

class AccountEditDialog::AccountEditDialogImpl
{
public:
    explicit AccountEditDialogImpl(AccountEditDialog *dialog, AccountsModel *model, ItemAction action): _dialog(dialog),
                                                                                                    _ui(new Ui::AccountEditDialog),
                                                                                                    _model(model),
                                                                                                    _action(action),
                                                                                                    _account(nullptr),
                                                                                                    _oParent(nullptr),
                                                                                                    _nParent(nullptr)
    {
        _ui->setupUi(_dialog);
        connect(_ui->btnOk, &QPushButton::clicked, _dialog, &AccountEditDialog::okHandler);
        connect(_ui->btnCancel, &QPushButton::clicked, _dialog, &AccountEditDialog::cancelHandler);
        connect(_ui->btnParent, &QPushButton::clicked, _dialog, &AccountEditDialog::parentSelectHandler);
        connect(_ui->btnReset, &QPushButton::clicked, _dialog, &AccountEditDialog::parentResetHandler);

        if(_action == ItemAction::ADD)
        {
            _account = std::shared_ptr<Account>(new Account());
            _dialog->setWindowTitle("Add Account");
        }
        else
        {
            _dialog->setWindowTitle("Add Account");
        }
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
            _oParent = _model->getAccount(index.parent());
            load();
        }
    }

    void resetParent()
    {
        _nParent = nullptr;
        _nParentIndex = QModelIndex();
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

        if(_oParent)
            _ui->txtParent->setText(_oParent->name());
    }

    bool checkRootParentLinkage()
    {
        if(_nParent == nullptr)
        {
            auto r = QMessageBox::question(_dialog,
                                           "No Parent Account",
                                           "You have not selected a parent account, if you continue this account will be linked at the root level.\n\nYES to continue, NO to stop.",
                                           QMessageBox::Yes | QMessageBox::No);
            return r == QMessageBox::Yes;
        }
        return true;
    }

    bool save()
    {
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
                try
                {
                    if(isParentChanging())
                    {
                        bool pass = checkRootParentLinkage();
                        if(pass)
                            _model->moveAccount(_context, _nParentIndex);
                        else
                            return false;
                    }
                    else
                    {
                        _model->updateAccount(_context);
                    }
                    return true;
                }
                catch(ObjectError err)
                {
                    QMessageBox::critical(_dialog, "Error Updating", QString(err.what()));
                }
            }
            case ItemAction::ADD:
            {
                try
                {
                    bool pass = checkRootParentLinkage();
                    if (pass)
                    {
                        _model->addAccount(*(_account.get()), _nParentIndex);
                        return true;
                    }
                    return false;
                }
                catch(ObjectError err)
                {
                    QMessageBox::critical(_dialog, "Error Adding", QString(err.what()));
                }
            }
            default:
                return false;
        }
    }

    bool isParentChanging()
    {
        if(!_oParent)  // if there is no original parent, i.e. new account, parent needs will need be set
            return true;

        if(_oParent && !_nParent) // if there was an orig parent and new parent is null, intent is to set parent to root as a change
            return true;

        return _oParent->key().compare(_nParent->key(), Qt::CaseSensitive) != 0;  // compare the acct keys for orig and new parent, if diff the parent is changing
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
            _nParentIndex = picker->getSelected();
            _nParent = _model->getAccount(_nParentIndex);
            if(_nParent)
            {
                if (AccountsModel::isRootAccount(_nParent))
                {
                    _nParent = nullptr;
                    _nParentIndex = QModelIndex();
                }
                else
                    _ui->txtParent->setText(_nParent->name());

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
    std::shared_ptr<Account> _oParent;
    std::shared_ptr<Account> _nParent;
    QModelIndex _nParentIndex;
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

void AccountEditDialog::load()
{
    impl->load();
}
