#include "ui/paymentspanel.h"
#include "ui_paymentspanel.h"
#include "ui/panelactions.h"

class PaymentsPanel::PaymentsPanelImpl
{
public:
    PaymentsPanelImpl(PaymentsPanel *panel): _panel(panel), _ui(new Ui::PaymentsPanel)
    {
        _ui->setupUi(_panel);
        ItemState initial;
        initial.setDeleteEnabled(false);
        initial.setSaveEnabled(true);
        initial.setEditEnabled(false);
        initial.setAddEnabled(true);
        setCurrentState(initial);
    }

    ~PaymentsPanelImpl()
    {
        delete _ui;
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
            case ItemAction::SAVE:
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

    void activate(bool active, PanelActions *actions)
    {
        if(active)
        {
            connect(actions->getAction(1), &QAction::triggered, _panel, &PaymentsPanel::finalizeHandler);
        }
        else
        {
            disconnect(actions->getAction(1), nullptr,_panel, nullptr);
        }
    }

    void finalize()
    {

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

private:
    PaymentsPanel *_panel;
    Ui::PaymentsPanel *_ui;
    ItemState _currentState;
};

PaymentsPanel::PaymentsPanel(QWidget *parent) : PanelWindow(parent),impl(new PaymentsPanelImpl(this)){}

PaymentsPanel::~PaymentsPanel(){}

QString PaymentsPanel::panelName() const
{
    return QString("Payments");
}

Panel PaymentsPanel::panelId() const
{
    return Panel::PAYMENTS;
}

ItemState PaymentsPanel::getCurrentState() const
{
    return impl->currentState();
}

void PaymentsPanel::activate(PanelActions *actions)
{
    impl->activate(true, actions);
}

void PaymentsPanel::deactivate(PanelActions *actions)
{
    impl->activate(false, actions);
}

void PaymentsPanel::itemActionHandler(ItemAction action)
{
    impl->processItemAction(action);
}

void PaymentsPanel::finalizeHandler()
{
    impl->finalize();
}
