#include "ui/accountpickerdialog.h"
#include "ui_accountpickerdialog.h"

class AccountPickerDialog::AccountPickerDialogImpl
{
public:
    AccountPickerDialogImpl(AccountsModel *model, AccountPickerDialog *dialog) : _ui(new Ui::AccountPickerDialog), _dialog(dialog), _model(model)
    {
        _ui->setupUi(_dialog);
        _proxy = new AccountsProxyModel(_dialog);
        _proxy->setSourceModel(_model);
        _ui->treeAccounts->setModel(_proxy);
        _ui->treeAccounts->expandAll();
        _ui->treeAccounts->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
        connect(_ui->btnOk, &QPushButton::clicked, _dialog, &AccountPickerDialog::okHandler);
        connect(_ui->btnCancel, &QPushButton::clicked, _dialog, &AccountPickerDialog::cancelHandler);
    }

    ~AccountPickerDialogImpl()
    {
        delete _ui;
    }

    void setContextItem(const QModelIndex& index)
    {
        _proxy->setModelIndexFilter(index);
    }

    QModelIndex selected()
    {
        QModelIndexList selected = _ui->treeAccounts->selectionModel()->selectedRows();
        if(selected.count() == 1)
        {
            return _proxy->mapToSource(selected.at(0));
        }
        else
        {
            return QModelIndex();
        }
    }

private:
    AccountPickerDialog *_dialog;
    Ui::AccountPickerDialog *_ui;
    AccountsModel *_model;
    AccountsProxyModel *_proxy;
};

AccountPickerDialog::AccountPickerDialog(AccountsModel *model, QWidget *parent) : impl(new AccountPickerDialogImpl(model, this))
{}

AccountPickerDialog::~AccountPickerDialog()
{}

void AccountPickerDialog::setContextItem(const QModelIndex &index)
{
    impl->setContextItem(index);
}

void AccountPickerDialog::okHandler()
{
    this->done(QDialog::Accepted);
    this->close();
}

void AccountPickerDialog::cancelHandler()
{
    this->done(QDialog::Rejected);
    this->close();
}

QModelIndex AccountPickerDialog::getSelected() const
{
    return impl->selected();
}
