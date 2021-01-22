#include "ui/fundsregisterpanel.h"
#include "ui_fundsregisterpanel.h"
#include "ui/panelactions.h"
#include "objects/fundsrecordsmodel.h"
#include "objects/objecterror.h"
#include "ui/fundsrecordeditdialog.h"
#include <QDebug>

class FundsRegisterPanel::FundsRegisterPanelImpl
{
public:
    FundsRegisterPanelImpl(FundsRegisterPanel *panel): _ui(new Ui::FundsRegisterPanel), _panel(panel)
    {
        _ui->setupUi(_panel);

        ItemState initial;
        initial.setDeleteEnabled(false);
        initial.setSaveEnabled(false);
        initial.setEditEnabled(false);
        initial.setAddEnabled(true);
        setCurrentState(initial);

        _model = new Transactions::FundsRecordsModel(_panel);
        _proxy = new Transactions::FundsRecordProxyModel(_panel);
        _proxy->setSourceModel(_model);
        _ui->tableFunds->setModel(_proxy);


        connect(_ui->tableFunds->selectionModel(), &QItemSelectionModel::selectionChanged, _panel, &FundsRegisterPanel::selectionChangedHandler);

        try
        {
            _model->load();
        }
        catch(ObjectError err)
        {
            QMessageBox::critical(_panel, "Error", QString("An error has occurred: \n %1").arg(err.what()));
        }
    }

    ~FundsRegisterPanelImpl()
    {
        delete _ui;
    }

    void processItemAction(ItemAction action)
    {
        switch(action)
        {
            case ItemAction::ADD:
            {
                Transactions::FundsRecord fr;
                FundsRecordEditDialog *dialog = new FundsRecordEditDialog(_panel);
                dialog->setModal(true);
                dialog->setRecord(&fr, UI::Action::ADD);
                int r = dialog->exec();
                delete dialog;

                if(r == QDialog::Accepted)
                {
                    try
                    {
                        _model->addRecord(fr);
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
                QModelIndexList selected = _ui->tableFunds->selectionModel()->selectedRows();
                if(selected.count() == 1)
                {
                    QModelIndex idx = _proxy->mapToSource(selected.at(0));
                    std::shared_ptr<Transactions::FundsRecord> fr = _model->getRecord(idx);
                    if(fr && !fr->deposited())
                    {
                        FundsRecordEditDialog *dialog = new FundsRecordEditDialog(_panel);
                        dialog->setModal(true);
                        dialog->setRecord(fr.get(), UI::Action::EDIT);
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
                        _ui->tableFunds->selectionModel()->clearSelection();
                    }
                    else
                    {
                        QMessageBox::critical(_panel, "Error", "deposited records can not be edited");
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
                QModelIndexList selected = _ui->tableFunds->selectionModel()->selectedRows();
                if(selected.count() == 1)
                {
                    QModelIndex idx = _proxy->mapToSource(selected.at(0));
                    std::shared_ptr<Transactions::FundsRecord> fr = _model->getRecord(idx);
                    if(fr && !fr->deposited())
                    {
                        QMessageBox msgBox;
                        msgBox.setText("Are you sure you would like to delete this record, this action is final and can not be reversed?");
                        msgBox.setInformativeText("Delete Record?");
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
                        _ui->tableFunds->selectionModel()->clearSelection();
                    }
                    else
                    {
                        QMessageBox::critical(_panel, "Error", "Deposited records can not be deleted");
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
            emit _panel->itemActionStateChange(_currentState);
        }
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

private:
    FundsRegisterPanel *_panel;
    Ui::FundsRegisterPanel *_ui;
    Transactions::FundsRecordsModel *_model;
    Transactions::FundsRecordProxyModel *_proxy;
    ItemState _currentState;
};

FundsRegisterPanel::FundsRegisterPanel(QWidget *parent) :
    PanelWindow(parent),
    impl(new FundsRegisterPanelImpl(this))
{}

FundsRegisterPanel::~FundsRegisterPanel()
{}

QString FundsRegisterPanel::panelName() const
{
    return QString("Funds Register");
}

Panel FundsRegisterPanel::panelId() const
{
    return Panel::FUNDS;
}

ItemState FundsRegisterPanel::getCurrentState() const
{
    return impl->currentState();
}

void FundsRegisterPanel::activate(PanelActions *actions)
{
    impl->activate(true, actions);
}

void FundsRegisterPanel::deactivate(PanelActions *actions)
{
    impl->activate(false, actions);
}

void FundsRegisterPanel::itemActionHandler(ItemAction action)
{
    impl->processItemAction(action);
}

void FundsRegisterPanel::selectionChangedHandler(const QItemSelection &selected, const QItemSelection &deselected)
{
    impl->selectionChanged(selected, deselected);
}

