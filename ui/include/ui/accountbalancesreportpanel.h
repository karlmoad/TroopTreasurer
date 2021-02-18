#ifndef ACCOUNTBALANCESREPORTPANEL_H
#define ACCOUNTBALANCESREPORTPANEL_H

#include <QWidget>
#include <memory>
#include "objects/objecterror.h"
#include "itemstate.h"
#include "panelwindow.h"

class AccountBalancesReportPanel : public PanelWindow
{
    Q_OBJECT

public:
    REM_COPY_MOVE(AccountBalancesReportPanel)
    explicit AccountBalancesReportPanel(QWidget *parent = nullptr);
    ~AccountBalancesReportPanel();

    virtual QString panelName() const override;

    virtual Panel panelId() const override;

    virtual ItemState getCurrentState() const override;

    virtual void activate(PanelActions *actions) override;

    virtual void deactivate(PanelActions *actions) override;

    Q_SLOT virtual void itemActionHandler(ItemAction action) override;

    Q_SLOT virtual void addDateActionHandler();

    Q_SLOT void showContextMenuHandler(const QPoint& point);

    Q_SLOT void copyHandler();

private:
    class AccountBalancesReportPanelImpl;
    std::shared_ptr<AccountBalancesReportPanelImpl> impl;
};

#endif // ACCOUNTBALANCESREPORTPANEL_H
