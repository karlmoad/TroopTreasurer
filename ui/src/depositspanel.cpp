#include "ui/depositspanel.h"
#include "ui_depositspanel.h"
#include "ui/panelactions.h"
#include "objects/depositsmodel.h"
#include "objects/objecterror.h"
#include <QDebug>

class DepositsPanel::DepositsPanelImpl
{
public:
    DepositsPanelImpl(DepositsPanel *panel): _panel(panel), _ui(new Ui::DepositsPanel)
    {
        _ui->setupUi(_panel);
        ItemState initial;
        initial.setDeleteEnabled(false);
        initial.setSaveEnabled(false);
        initial.setEditEnabled(false);
        initial.setAddEnabled(true);
        setCurrentState(initial);
    }

    ~DepositsPanelImpl()
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

    void activate(bool active, PanelActions *actions)
    {
        if(active)
        {
            connect(actions->getAction(1), &QAction::triggered, _panel, &DepositsPanel::finalizeHandler);
            connect(actions->getAction(2), &QAction::triggered, _panel, &DepositsPanel::printSlipHandler);
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

private:
    DepositsPanel *_panel;
    Ui::DepositsPanel *_ui;
    ItemState _currentState;
};

DepositsPanel::DepositsPanel(QWidget *parent) :
    PanelWindow(parent),
    impl(new DepositsPanelImpl(this)){}

DepositsPanel::~DepositsPanel(){}

QString DepositsPanel::panelName() const
{
    return QString("Deposits");
}

Panel DepositsPanel::panelId() const
{
    return Panel::DEPOSITS;
}

ItemState DepositsPanel::getCurrentState() const
{
    return impl->currentState();
}

void DepositsPanel::activate(PanelActions *actions)
{
    impl->activate(true, actions);
}

void DepositsPanel::deactivate(PanelActions *actions)
{
    impl->activate(false, actions);
}

void DepositsPanel::itemActionHandler(ItemAction action)
{
    impl->processItemAction(action);
}

void DepositsPanel::finalizeHandler()
{

}

void DepositsPanel::printSlipHandler()
{

}
