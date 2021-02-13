#include "ui/accountbalancesreportpanel.h"
#include "ui_accountbalancesreportpanel.h"
#include "ui/panelactions.h"
#include "objects/accountbalancereportmodel.h"
#include "ui/datepickerdialog.h"
#include <QMessageBox>
#include <QDebug>

class AccountBalancesReportPanel::AccountBalancesReportPanelImpl
{
public:
    AccountBalancesReportPanelImpl(AccountBalancesReportPanel *panel): _panel(panel), _ui(new Ui::AccountBalancesReportPanel)
    {
        _ui->setupUi(_panel);
        _model = new AccountBalanceReportModel(_panel);
        try
        {
            _model->runReport();
            _ui->treeBalances->setModel(_model);
            _ui->treeBalances->header()->setSectionResizeMode(QHeaderView::Stretch);
            _ui->treeBalances->setContextMenuPolicy(Qt::CustomContextMenu);
            _actCopy = new QAction("Copy Selected", _panel);
            connect(_ui->treeBalances, &QTreeView::customContextMenuRequested, _panel, &AccountBalancesReportPanel::showContextMenuHandler);
            connect(_actCopy, &QAction::triggered, _panel, &AccountBalancesReportPanel::copyHandler);

            int unassAcct = AccountBalanceReportModel::getUnassociatedAccountCount();
            if(unassAcct != 0)
            {
                if(unassAcct > 0)
                {
                    QMessageBox::information(_panel, "Info", QString("There are currently %1 unassociated account, this can effect the balance calculations.").arg(QString::number(unassAcct)));
                }
                else
                {
                    QMessageBox::critical(_panel, "Error", "Unable to retrieve unassociated account number, please verify using account management panel");
                }
            }
        }
        catch(ObjectError err)
        {
            QMessageBox::critical(_panel, "Error", "Error loading report" + QString(err.what()));
        }
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

    void addDate()
    {
        DatePickerDialog *dialog = new DatePickerDialog(_panel);
        dialog->setModal(true);
        int r = dialog->exec();
        if(r == QDialog::Accepted)
        {
            _model->addDate(dialog->selected());
        }
        delete dialog;
    }

    void showMenu(const QPoint& p)
    {
        QModelIndexList selected = _ui->treeBalances->selectionModel()->selectedIndexes();
        if(selected.count() > 0)
        {
            QList<QAction*> actions;
            actions.append(_actCopy);
            QMenu::exec(actions, _ui->treeBalances->mapToGlobal(p), nullptr, _panel);
        }
    }

    void copy()
    {
        qDebug() << "Copy called";
    }

private:
    AccountBalancesReportPanel *_panel;
    Ui::AccountBalancesReportPanel *_ui;
    AccountBalanceReportModel *_model;
    QAction *_actCopy;
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
    impl->addDate();
}

void AccountBalancesReportPanel::copyHandler()
{
    impl->copy();
}

void AccountBalancesReportPanel::showContextMenuHandler(const QPoint &point)
{
    impl->showMenu(point);
}
