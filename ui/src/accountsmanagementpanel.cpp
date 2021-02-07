#include "ui/accountsmanagementpanel.h"
#include "ui_accountsmanagementpanel.h"
#include "ui/panelactions.h"
#include "objects/objecterror.h"
#include "objects/accountsmodel.h"

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

        _model = new AccountsModel(_panel);
        _ui->treeAccounts->setModel(_model);

        connect(_ui->treeAccounts->selectionModel(), &QItemSelectionModel::selectionChanged, _panel, &AccountsManagementPanel::selectionChangedHandler);

        try
        {
            _model->load();
        }
        catch(ObjectError err)
        {
            QMessageBox::critical(_panel, "Error", QString("An error has occurred: \n %1").arg(err.what()));
        }
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
        if(!selected.isEmpty() && selected.indexes().length() == 1)
        {
            std::shared_ptr<Account> acct = _model->getAccount(selected.indexes()[0]);
            if(acct)
            {
                ItemState state = currentState();
                state.setEditEnabled(true);
                state.setDeleteEnabled(acct->isClosed());
                setCurrentState(state);
            }
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
                break;
            }
            case ItemAction::EDIT:
            {

                break;
            }
            case ItemAction::DELETE:
            {
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
