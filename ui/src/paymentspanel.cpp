#include "ui/paymentspanel.h"
#include "ui_paymentspanel.h"
#include "ui/panelactions.h"
#include "objects/paymentsmodel.h"
#include "objects/objecterror.h"
#include "ui/paymenteditdialog.h"

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

        _model = new Transactions::PaymentsModel(_panel);
        _proxy = new Transactions::PaymentsProxyModel(_panel);
        _proxy->setSourceModel(_model);
        _ui->tablePayments->setModel(_proxy);
        _proxy->setFinalizedStatusFilter(true);
        _proxy->setActiveStatusFilter(true);

        try
        {
            _model->load(DateLimits::MinDate, DateLimits::MaxDate);
        }
        catch(ObjectError err)
        {
            QMessageBox::critical(_panel, "Error", QString("An error has occurred: \n %1").arg(err.what()));
        }
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
                Transactions::Payment p(_panel);
                PaymentEditDialog *dialog = new PaymentEditDialog(_panel);
                dialog->setModal(true);
                dialog->setRecord(&p);
                dialog->setAction(UI::Action::ADD);
                int r = dialog->exec();

                if(r == QDialog::Accepted)
                {
                    qDebug() << "Date: " << p.date() << " Amount: " << p.amount();
                    //_model->addPayment(p);
                }
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
    Transactions::PaymentsModel *_model;
    Transactions::PaymentsProxyModel *_proxy;
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
