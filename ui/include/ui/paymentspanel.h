#ifndef PAYMENTSPANEL_H
#define PAYMENTSPANEL_H

#include <QWidget>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <memory>
#include "itemstate.h"
#include "panelwindow.h"

class PaymentsPanel : public PanelWindow
{
    Q_OBJECT
public:
    virtual QString panelName() const override;

    virtual Panel panelId() const override;

    virtual ItemState getCurrentState() const override;

    virtual void activate(PanelActions *actions) override;

    virtual void deactivate(PanelActions *actions) override;

    Q_SLOT virtual void itemActionHandler(ItemAction action) override;

    Q_SLOT void finalizeHandler();

    Q_SLOT void changeQueueDepthUpdateHandler(int count);

    Q_SLOT void selectionChangedHandler(const QItemSelection &selected, const QItemSelection &deselected);

public:
    explicit PaymentsPanel(QWidget *parent = nullptr);
    ~PaymentsPanel();

private:
    class PaymentsPanelImpl;
    std::shared_ptr<PaymentsPanelImpl> impl;
};

#endif // PAYMENTSPANEL_H
