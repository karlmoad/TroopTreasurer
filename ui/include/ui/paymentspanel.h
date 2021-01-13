#ifndef PAYMENTSPANEL_H
#define PAYMENTSPANEL_H

#include <QWidget>
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

public:
    explicit PaymentsPanel(QWidget *parent = nullptr);
    ~PaymentsPanel();

private:
    class PaymentsPanelImpl;
    std::shared_ptr<PaymentsPanelImpl> impl;
};

#endif // PAYMENTSPANEL_H
