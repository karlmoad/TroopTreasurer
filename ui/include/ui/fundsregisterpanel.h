#ifndef FUNDSREGISTERPANEL_H
#define FUNDSREGISTERPANEL_H

#include <QWidget>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <memory>
#include "itemstate.h"
#include "panelwindow.h"

class FundsRegisterPanel : public PanelWindow
{
Q_OBJECT
public:
    explicit FundsRegisterPanel(QWidget *parent = nullptr);
    ~FundsRegisterPanel();

    virtual QString panelName() const override;

    virtual Panel panelId() const override;

    virtual ItemState getCurrentState() const override;

    virtual void activate(PanelActions *actions) override;

    virtual void deactivate(PanelActions *actions) override;

    Q_SLOT virtual void itemActionHandler(ItemAction action) override;
    Q_SLOT void selectionChangedHandler(const QItemSelection &selected, const QItemSelection &deselected);

private:
    class FundsRegisterPanelImpl;
    std::shared_ptr<FundsRegisterPanelImpl> impl;
};

#endif // FUNDSREGISTERPANEL_H
