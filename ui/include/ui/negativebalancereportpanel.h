#ifndef NEGATIVEBALANCEREPORTPANEL_H
#define NEGATIVEBALANCEREPORTPANEL_H

#include <QWidget>
#include <memory>
#include "objects/objecterror.h"
#include "objects/negativebalancereportmodel.h"
#include "itemstate.h"
#include "panelwindow.h"

class NegativeBalanceReportPanel : public PanelWindow
{
    Q_OBJECT
public:
    REM_COPY_MOVE(NegativeBalanceReportPanel)
    explicit NegativeBalanceReportPanel(QWidget *parent = nullptr);
    ~NegativeBalanceReportPanel();

    virtual QString panelName() const;

    virtual Panel panelId() const;

    virtual ItemState getCurrentState() const;

    virtual void activate(PanelActions *actions);

    virtual void deactivate(PanelActions *actions);

    Q_SLOT virtual void itemActionHandler(ItemAction action);

private:
    class NegativeBalanceReportPanelImpl;
    std::shared_ptr<NegativeBalanceReportPanelImpl> impl;
};

#endif // NEGATIVEBALANCEREPORTPANEL_H
