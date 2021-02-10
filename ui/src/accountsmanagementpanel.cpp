#include "ui/accountsmanagementpanel.h"
#include "ui_accountsmanagementpanel.h"
#include "ui/panelactions.h"
#include "ui/accounteditdialog.h"
#include "objects/objecterror.h"
#include "objects/accountsmodel.h"
#include <QTreeView>
#include <QMessageBox>
#include <QItemSelectionModel>
#include <QHeaderView>

class AccountsManagementPanel::AccountsManagementPanelImpl
{
public:
    AccountsManagementPanelImpl(AccountsManagementPanel *panel): _panel(panel), _ui(new Ui::AccountsManagementPanel)
    {
        _ui->setupUi(_panel);
        ItemState initial;
        initial.setDeleteEnabled(false);
        initial.setSaveEnabled(false);
        initial.setEditEnabled(false);
        initial.setAddEnabled(true);
        setCurrentState(initial);

        try
        {
            _model = new AccountsModel(_panel);
            _model->load();
            _proxy = new AccountsProxyModel(_panel);
            _proxy->setSourceModel(_model);
            _ui->treeAccounts->setModel(_proxy);
            _ui->chkShowClosed->setCheckState(Qt::Unchecked);

            connect(_model, &QAbstractItemModel::rowsInserted, _panel, &AccountsManagementPanel::rowsInsertedHandler);
            connect(_model, &QAbstractItemModel::rowsMoved, _panel, &AccountsManagementPanel::rowsMovedHandler);
            connect(_ui->treeAccounts, &QTreeView::expanded, _panel, &AccountsManagementPanel::itemExpandedHandler);
            connect(_ui->treeAccounts->selectionModel(), &QItemSelectionModel::selectionChanged, _panel, &AccountsManagementPanel::selectionChangedHandler);
            connect(_ui->chkShowClosed, &QCheckBox::stateChanged, _panel, &AccountsManagementPanel::showClosedToggleHandler);
            setShowClosedAccounts();
            _ui->treeAccounts->resizeColumnToContents(0);
        }
        catch(ObjectError err)
        {
            QMessageBox::critical(_panel, "Error", QString("An error has occurred: \n %1").arg(err.what()));
        }
    }

    void setShowClosedAccounts()
    {
        _proxy->setAccountClosedViewFilter(_ui->chkShowClosed->isChecked());
    }

    void expandItem(const QModelIndex& index, bool proxied = false)
    {
        QModelIndex idx = index;
        if(proxied)
        {
            idx = _proxy->mapFromSource(index);
        }

        _ui->treeAccounts->expand(idx);
        if(idx.parent().isValid())
        {
            expandItem(idx.parent());
        }
    }

    void itemExpanded(const QModelIndex &index)
    {
        _ui->treeAccounts->resizeColumnToContents(0);
    }

    void userResized(int logicalIndex, int oldSize, int newSize)
    {
        qDebug() << "resized by user";
    }

    void activate(bool active, PanelActions *actions)
    {
        if(active)
        {
            connect(actions->getAction(1), &QAction::triggered, _panel, &AccountsManagementPanel::viewOrphanSourceAccounts);
            emit _panel->itemActionStateChange(_currentState);
        }
        else
        {
            for(int i: actions->getActionIdentifiers())
            {
                disconnect(actions->getAction(i), nullptr, _panel, nullptr);
            }
        }
    }

    ~AccountsManagementPanelImpl()
    {
        delete _ui;
    }

    ItemState currentState()
    {
        return _currentState;
    }

    void setCurrentState(ItemState state)
    {
        _currentState = state;
        emit _panel->itemActionStateChange(_currentState);
    }

    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
    {
        if(!selected.isEmpty())
        {
            ItemState state = currentState();
            state.setEditEnabled(true);
            state.setDeleteEnabled(true);
            setCurrentState(state);
        }
        else
        {
            ItemState state = currentState();
            state.setEditEnabled(false);
            state.setDeleteEnabled(false);
            setCurrentState(state);
        }
    }

    void processItemAction(ItemAction action)
    {
        switch(action)
        {
            case ItemAction::ADD:
            {
                AccountEditDialog *dialog = new AccountEditDialog(_model, ItemAction::ADD, _panel);
                dialog->setModal(true);
                dialog->exec();
                delete dialog;
                break;
            }
            case ItemAction::EDIT:
            {
                QModelIndexList selected = _ui->treeAccounts->selectionModel()->selectedRows();
                if(selected.count() ==1)
                {
                    AccountEditDialog *dialog = new AccountEditDialog(_model, ItemAction::EDIT, _panel);
                    dialog->setModal(true);
                    dialog->setContextItem(_proxy->mapToSource(selected.at(0)));
                    dialog->exec();
                    delete dialog;
                }

                break;
            }
            case ItemAction::DELETE:
            {
                QModelIndexList selected = _ui->treeAccounts->selectionModel()->selectedRows();
                if(selected.count() ==1)
                {
                    std::shared_ptr<Account> acct = _model->getAccount(_proxy->mapToSource(selected.at(0)));
                    if(acct && acct->isClosed())
                    {
                        QMessageBox msgBox;
                        msgBox.setText(
                                "Are you sure you would like to delete this Account, this action is final and can not be reversed?");
                        msgBox.setInformativeText("Delete Account?");
                        msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
                        msgBox.setDefaultButton(QMessageBox::No);
                        int r = msgBox.exec();

                        if (r == QMessageBox::Yes)
                        {
                            try
                            {
                                _model->deleteAccount(_proxy->mapToSource(selected.at(0)));
                            }
                            catch (ObjectError err)
                            {
                                QMessageBox::critical(_panel, "Error", QString(err.what()));
                            }
                        }
                    }
                    else
                    {
                        QMessageBox::critical(_panel, "Error", "Account must be closed to delete");
                    }
                }
                break;
            }
            default:
                break;
        }
    }

private:
    AccountsManagementPanel *_panel;
    Ui::AccountsManagementPanel *_ui;
    AccountsModel *_model;
    AccountsProxyModel *_proxy;
    ItemState _currentState;
};

AccountsManagementPanel::AccountsManagementPanel(QWidget *parent) : PanelWindow(parent), impl(new AccountsManagementPanelImpl(this)){}

AccountsManagementPanel::~AccountsManagementPanel(){}

QString AccountsManagementPanel::panelName() const
{
    return QString("Accounts Management");
}

Panel AccountsManagementPanel::panelId() const
{
    return Panel::ACCOUNTSMGMT;
}

ItemState AccountsManagementPanel::getCurrentState() const
{
    return ItemState();
}

void AccountsManagementPanel::activate(PanelActions *actions)
{
    impl->activate(true, actions);
}

void AccountsManagementPanel::deactivate(PanelActions *actions)
{
    impl->activate(false, actions);
}

void AccountsManagementPanel::itemActionHandler(ItemAction action)
{
    impl->processItemAction(action);
}

void AccountsManagementPanel::selectionChangedHandler(const QItemSelection &selected, const QItemSelection &deselected)
{
    impl->selectionChanged(selected, deselected);
}

void AccountsManagementPanel::viewOrphanSourceAccounts()
{

}

void AccountsManagementPanel::rowsInsertedHandler(const QModelIndex &parent, int first, int last)
{
    impl->expandItem(parent, true);
}

void AccountsManagementPanel::rowsMovedHandler(const QModelIndex &parent, int start, int end, const QModelIndex &destination,int row)
{
    impl->expandItem(destination, true);
}

void AccountsManagementPanel::showClosedToggleHandler()
{
    impl->setShowClosedAccounts();
}

void AccountsManagementPanel::sectionResizedHandler(int logicalIndex, int oldSize, int newSize)
{
    impl->userResized(logicalIndex, oldSize, newSize);
}

void AccountsManagementPanel::itemExpandedHandler(const QModelIndex &index)
{
    impl->itemExpanded(index);
}
