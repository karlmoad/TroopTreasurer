#include <QString>
#include <ui/accountsmanagementpanel.h>
#include "ui/negativebalancereportpanel.h"
#include "ui_negativebalancereportpanel.h"

class NegativeBalanceReportPanel::NegativeBalanceReportPanelImpl
{
public:
    NegativeBalanceReportPanelImpl(NegativeBalanceReportPanel* panel): _panel(panel),
                                                                        _ui(new Ui::NegativeBalanceReportPanel),
                                                                        _model(new NegativeBalanceReportModel(_panel))
    {
        _ui->setupUi(_panel);
        _ui->tableReport->setModel(_model);
    }

    void load()
    {
        _model->runReport();
        _ui->tableReport->resizeColumnToContents(0);
        _ui->tableReport->resizeColumnToContents(1);
    }

private:
    NegativeBalanceReportPanel *_panel;
    Ui::NegativeBalanceReportPanel *_ui;
    NegativeBalanceReportModel *_model;
};

NegativeBalanceReportPanel::NegativeBalanceReportPanel(QWidget *parent) : PanelWindow(parent), impl(new NegativeBalanceReportPanelImpl(this))
{
    impl->load();
}

NegativeBalanceReportPanel::~NegativeBalanceReportPanel()
{
}

QString NegativeBalanceReportPanel::panelName() const
{
    return "Negative Balance Report";
}

Panel NegativeBalanceReportPanel::panelId() const
{
    return Panel::NEGBALREPORT;
}

ItemState NegativeBalanceReportPanel::getCurrentState() const
{
    ItemState state;
    state.setSaveEnabled(false);
    state.setAddEnabled(false);
    state.setEditEnabled(false);
    state.setDeleteEnabled(false);
    return state;
}

void NegativeBalanceReportPanel::activate(PanelActions *actions)
{
    return; // this panel has no valid actions, noting to modify
}

void NegativeBalanceReportPanel::deactivate(PanelActions *actions)
{
    return; // this panel has no valid actions, noting to modify
}

void NegativeBalanceReportPanel::itemActionHandler(ItemAction action)
{
    return; // this panel has no valid actions
}
