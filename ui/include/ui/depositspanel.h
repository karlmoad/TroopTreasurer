#ifndef DEPOSITSPANEL_H
#define DEPOSITSPANEL_H

#include <QWidget>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <memory>
#include "itemstate.h"
#include "panelwindow.h"

class DepositsPanel : public PanelWindow
{
    Q_OBJECT
public:
    REM_COPY_MOVE(DepositsPanel)
    explicit DepositsPanel(QWidget *parent = nullptr);
    ~DepositsPanel();

    virtual QString panelName() const override;

    virtual Panel panelId() const override;

    virtual ItemState getCurrentState() const override;

    virtual void activate(PanelActions *actions) override;

    virtual void deactivate(PanelActions *actions) override;

    Q_SLOT virtual void itemActionHandler(ItemAction action) override;

    Q_SLOT void finalizeHandler();

    Q_SLOT void printSlipHandler();

    Q_SLOT void viewHandler();

    Q_SLOT void selectionChangedHandler(const QItemSelection &selected, const QItemSelection &deselected);

private:
    class DepositsPanelImpl;
    std::shared_ptr<DepositsPanelImpl> impl;
};

#endif // DEPOSITSPANEL_H
