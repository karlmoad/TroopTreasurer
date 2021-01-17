#include "ui/paymentspanel.h"
#include "ui_paymentspanel.h"
#include "ui/panelactions.h"
#include "objects/paymentsmodel.h"
#include "objects/objecterror.h"
#include "ui/paymenteditdialog.h"
#include <QDebug>

class PaymentsPanel::PaymentsPanelImpl
{
public:
    PaymentsPanelImpl(PaymentsPanel *panel): _panel(panel), _ui(new Ui::PaymentsPanel)
    {
        _ui->setupUi(_panel);
        ItemState initial;
        initial.setDeleteEnabled(false);
        initial.setSaveEnabled(false);
        initial.setEditEnabled(false);
        initial.setAddEnabled(true);
        setCurrentState(initial);

        _model = new Transactions::PaymentsModel(_panel);
        _proxy = new Transactions::PaymentsProxyModel(_panel);
        _proxy->setSourceModel(_model);
        _ui->tablePayments->setModel(_proxy);
        _proxy->setFinalizedStatusFilter(true);
        _proxy->setActiveStatusFilter(true);

        connect(_ui->tablePayments->selectionModel(), &QItemSelectionModel::selectionChanged, _panel, &PaymentsPanel::selectionChangedHandler);

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
                Transactions::Payment p;
                PaymentEditDialog *dialog = new PaymentEditDialog(_panel);
                dialog->setModal(true);
                dialog->setRecord(&p, UI::Action::ADD);
                int r = dialog->exec();
                delete dialog;

                if(r == QDialog::Accepted)
                {
                    try
                    {
                        _model->addPayment(p);
                    }
                    catch(ObjectError err)
                    {
                        QMessageBox::critical(_panel, "Error", QString(err.what()));
                    }
                }
                break;
            }
            case ItemAction::EDIT:
            {
                QModelIndexList selected = _ui->tablePayments->selectionModel()->selectedRows();
                if(selected.count() == 1)
                {
                    QModelIndex idx = _proxy->mapToSource(selected.at(0));
                    std::shared_ptr<Transactions::Payment> p = _model->getPayment(idx);
                    if(p && !p->finalized())
                    {
                        PaymentEditDialog *dialog = new PaymentEditDialog(_panel);
                        dialog->setModal(true);
                        dialog->setRecord(p.get(), UI::Action::EDIT);
                        int r = dialog->exec();
                        delete dialog;

                        if (r == QDialog::Accepted)
                        {
                            try
                            {
                                _model->updateRecord(idx);
                            }
                            catch(ObjectError err)
                            {
                                QMessageBox::critical(_panel, "Error", QString(err.what()));
                            }
                        }
                        _ui->tablePayments->selectionModel()->clearSelection();
                    }
                    else
                    {
                        QMessageBox::critical(_panel, "Error", "Finalized records can not be edited");
                    }
                }
                break;
            }
            case ItemAction::SAVE:
            {
                break;
            }
            case ItemAction::DELETE:
            {
                QModelIndexList selected = _ui->tablePayments->selectionModel()->selectedRows();
                if(selected.count() == 1)
                {
                    QModelIndex idx = _proxy->mapToSource(selected.at(0));
                    std::shared_ptr<Transactions::Payment> p = _model->getPayment(idx);
                    if(p && !p->finalized())
                    {
                        QMessageBox msgBox;
                        msgBox.setText("Are you sure you would like to delete this Payment, this action is final and can not be reversed?");
                        msgBox.setInformativeText("Delete Payment?");
                        msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
                        msgBox.setDefaultButton(QMessageBox::No);
                        int r = msgBox.exec();

                        if (r == QMessageBox::Yes)
                        {
                            try
                            {
                                _model->deleteRecord(idx);
                            }
                            catch(ObjectError err)
                            {
                                QMessageBox::critical(_panel, "Error", QString(err.what()));
                            }
                        }
                        _ui->tablePayments->selectionModel()->clearSelection();
                    }
                    else
                    {
                        QMessageBox::critical(_panel, "Error", "Finalized records can not be deleted");
                    }
                }
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
            connect(actions->getAction(2), &QAction::triggered, _panel, &PaymentsPanel::filterDatesHandler);
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

    void finalize()
    {
        QModelIndexList selected = _ui->tablePayments->selectionModel()->selectedRows();
        if(selected.count() == 1)
        {
            QModelIndex idx = _proxy->mapToSource(selected.at(0));
            std::shared_ptr<Transactions::Payment> p = _model->getPayment(idx);
            if(p)
            {
                p->setFinalized(true);
                try
                {
                    _model->updateRecord(idx);
                    _ui->tablePayments->selectionModel()->clearSelection();
                }
                catch(ObjectError err)
                {
                    QMessageBox::critical(_panel, "Error", QString(err.what()));
                }
            }
        }
    }

    void filterDates()
    {
        qDebug() << "Filter";
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

    void updateChangeQueueDepth(int count)
    {
        if(count > 0)
        {
            _ui->lblAlerts->setText(
                    QString("(%1) Unsaved Change%2").arg(QString::number(count), (count > 1 ? "s" : "")));
        }
        else
        {
            _ui->lblAlerts->setText("");
        }
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

void PaymentsPanel::selectionChangedHandler(const QItemSelection &selected, const QItemSelection &deselected)
{
    impl->selectionChanged(selected,deselected);
}

void PaymentsPanel::filterDatesHandler()
{
    impl->filterDates();
}
