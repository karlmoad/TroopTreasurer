#include <QString>
#include <QApplication>
#include <QClipboard>
#include "objects/negativebalancereportmodel.h"
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
        _ui->tableReport->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        _ui->tableReport->setContextMenuPolicy(Qt::CustomContextMenu);
        _actCopy = new QAction("Copy Selected", _panel);
        connect(_ui->tableReport, &QTableView::customContextMenuRequested, _panel, &NegativeBalanceReportPanel::showContextMenuHandler);
        connect(_actCopy, &QAction::triggered, _panel, &NegativeBalanceReportPanel::copyHandler);
    }

    void load()
    {
        _model->runReport();
        _ui->tableReport->resizeColumnToContents(0);
        _ui->tableReport->resizeColumnToContents(1);
    }

    void showMenu(const QPoint& p)
    {
        QModelIndexList selected = _ui->tableReport->selectionModel()->selectedIndexes();
        if(selected.count() > 0)
        {
            QList<QAction*> actions;
            actions.append(_actCopy);
            QMenu::exec(actions, _ui->tableReport->mapToGlobal(p), nullptr, _panel);
        }
    }

    void copy()
    {
        QString buffer;
        int col =-1;
        QModelIndexList selected = _ui->tableReport->selectionModel()->selectedIndexes();
        if(selected.count() > 0)
        {
            for (const QModelIndex &c : selected)
            {
                QString data = _model->data(c, Qt::DisplayRole).toString();
                if (c.column() > col)
                {
                    buffer.append(QString("%1\t").arg(data));
                } else
                {
                    buffer.append(QString("\n%1\t").arg(data));
                }
                col = c.column();
            }
            QApplication::clipboard()->setText(buffer);
        }
    }


private:
    NegativeBalanceReportPanel *_panel;
    Ui::NegativeBalanceReportPanel *_ui;
    NegativeBalanceReportModel *_model;
    QAction *_actCopy;
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

void NegativeBalanceReportPanel::showContextMenuHandler(const QPoint &point)
{
    impl->showMenu(point);
}

void NegativeBalanceReportPanel::copyHandler()
{
    impl->copy();
}
