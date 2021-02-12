#include "ui/accountbalancesreportpanel.h"
#include "ui_accountbalancesreportpanel.h"
#include "ui/panelactions.h"

class AccountBalancesReportPanel::AccountBalancesReportPanelImpl
{
public:
    AccountBalancesReportPanelImpl(AccountBalancesReportPanel *panel): _panel(panel), _ui(new Ui::AccountBalancesReportPanel)
    {
        _ui->setupUi(_panel);
    }

    ~AccountBalancesReportPanelImpl()
    {
        delete _ui;
    }

    void activate(bool active, PanelActions *actions)
    {
        if(active)
        {
            ItemState state;
            state.setSaveEnabled(false);
            state.setAddEnabled(false);
            state.setEditEnabled(false);
            state.setDeleteEnabled(false);

            connect(actions->getAction(1), &QAction::triggered, _panel, &AccountBalancesReportPanel::addDateActionHandler);
            emit _panel->itemActionStateChange(state);
        }
        else
        {
            for(int i: actions->getActionIdentifiers())
            {
                disconnect(actions->getAction(i), nullptr, _panel, nullptr);
            }
        }
    }


private:
    AccountBalancesReportPanel *_panel;
    Ui::AccountBalancesReportPanel *_ui;

};

AccountBalancesReportPanel::AccountBalancesReportPanel(QWidget *parent) :
    PanelWindow(parent), impl(new AccountBalancesReportPanelImpl(this)){}

AccountBalancesReportPanel::~AccountBalancesReportPanel()
{}

QString AccountBalancesReportPanel::panelName() const
{
    return "Account Balances Report";
}

Panel AccountBalancesReportPanel::panelId() const
{
    return Panel::ACCTBALREPORT;
}

ItemState AccountBalancesReportPanel::getCurrentState() const
{
    ItemState state;
    state.setSaveEnabled(false);
    state.setAddEnabled(false);
    state.setEditEnabled(false);
    state.setDeleteEnabled(false);
    return state;
}

void AccountBalancesReportPanel::activate(PanelActions *actions)
{
    impl->activate(true, actions);
}

void AccountBalancesReportPanel::deactivate(PanelActions *actions)
{
    impl->activate(false,  actions);
}

void AccountBalancesReportPanel::itemActionHandler(ItemAction action)
{
    return;
}

void AccountBalancesReportPanel::addDateActionHandler()
{

}
