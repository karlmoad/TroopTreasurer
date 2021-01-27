#include "ui/depositspanel.h"
#include "ui_depositspanel.h"
#include "ui/panelactions.h"
#include "objects/depositsmodel.h"
#include "objects/objecterror.h"
#include "ui/depositeditdialog.h"
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

        _model = new Transactions::DepositsModel(_panel);
        _proxy = new Transactions::DepositsProxyModel(_panel);
        _proxy->setSourceModel(_model);
        _ui->tableDeposits->setModel(_proxy);

        connect(_ui->tableDeposits->selectionModel(), &QItemSelectionModel::selectionChanged, _panel, &DepositsPanel::selectionChangedHandler);

        try
        {
            _model->load(DateLimits::MinDate, DateLimits::MaxDate);
        }
        catch(ObjectError err)
        {
            QMessageBox::critical(_panel, "Error", QString("An error has occurred: \n %1").arg(err.what()));
        }
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
            connect(actions->getAction(1), &QAction::triggered, _panel, &DepositsPanel::viewHandler);
            connect(actions->getAction(2), &QAction::triggered, _panel, &DepositsPanel::finalizeHandler);
            connect(actions->getAction(3), &QAction::triggered, _panel, &DepositsPanel::printSlipHandler);
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
        QModelIndexList selected = _ui->tableDeposits->selectionModel()->selectedRows();
        if(selected.count() == 1)
        {
            QModelIndex idx = _proxy->mapToSource(selected.at(0));
            std::shared_ptr<Transactions::Deposit> d = _model->getDeposit(idx);
            if(d)
            {
                d->setFinalized(true);
                try
                {
                    _model->updateRecord(idx);
                    _ui->tableDeposits->selectionModel()->clearSelection();
                }
                catch(ObjectError err)
                {
                    QMessageBox::critical(_panel, "Error", QString(err.what()));
                }
            }
        }
    }

    void view()
    {
        QModelIndexList selected = _ui->tableDeposits->selectionModel()->selectedRows();
        if(selected.count() == 1)
        {
            QModelIndex idx = _proxy->mapToSource(selected.at(0));
            std::shared_ptr<Transactions::Deposit> d = _model->getDeposit(idx);

            DepositEditDialog *dialog = new DepositEditDialog(_panel);
            dialog->setModal(true);
            dialog->setDeposit(d, UI::Action::NONE);
            dialog->exec();
            delete dialog;
        }
        else
        {
            QMessageBox::critical(_panel, "Error", "Please select only one record");
        }
    }


    void processItemAction(ItemAction action)
    {
        switch(action)
        {
            case ItemAction::ADD:
            {
                Transactions::Deposit d;
                d.setDate(QDate::currentDate());
                d.setTotal(0.0);
                try
                {
                    _model->addDeposit(d);

                }
                catch(ObjectError err)
                {
                    QMessageBox::critical(_panel, "Error", QString(err.what()));
                }
                break;
            }
            case ItemAction::EDIT:
            {
                QModelIndexList selected = _ui->tableDeposits->selectionModel()->selectedRows();
                if(selected.count() == 1)
                {
                    QModelIndex idx = _proxy->mapToSource(selected.at(0));
                    std::shared_ptr<Transactions::Deposit> d = _model->getDeposit(idx);
                    if(d && !d->finalized())
                    {
                        DepositEditDialog *dialog = new DepositEditDialog(_panel);
                        dialog->setModal(true);
                        dialog->setDeposit(d, UI::Action::EDIT);
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
                        _ui->tableDeposits->selectionModel()->clearSelection();
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
            default:
                break;
        }
    }

    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
    {
        if(!selected.isEmpty())
        {
            ItemState state = currentState();
            state.setEditEnabled(true);
            setCurrentState(state);
        }
        else
        {
            ItemState state = currentState();
            state.setEditEnabled(false);
            setCurrentState(state);
        }
    }

private:
    DepositsPanel *_panel;
    Ui::DepositsPanel *_ui;
    ItemState _currentState;
    Transactions::DepositsModel *_model;
    Transactions::DepositsProxyModel *_proxy;
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
    impl->finalize();
}

void DepositsPanel::printSlipHandler()
{

}

void DepositsPanel::viewHandler()
{
    impl->view();
}

void DepositsPanel::selectionChangedHandler(const QItemSelection &selected, const QItemSelection &deselected)
{
    impl->selectionChanged(selected,deselected);
}
