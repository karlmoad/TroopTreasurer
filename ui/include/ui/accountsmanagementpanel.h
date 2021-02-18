#ifndef ACCOUNTSMANAGEMENTPANEL_H
#define ACCOUNTSMANAGEMENTPANEL_H

#include <QWidget>
#include <QItemSelection>
#include <memory>
#include "itemstate.h"
#include "panelwindow.h"

class AccountsManagementPanel : public PanelWindow
{
    Q_OBJECT
public:
    REM_COPY_MOVE(AccountsManagementPanel)
    explicit AccountsManagementPanel(QWidget *parent = nullptr);
    ~AccountsManagementPanel();

    virtual QString panelName() const override;

    virtual Panel panelId() const override;

    virtual ItemState getCurrentState() const override;

    virtual void activate(PanelActions *actions) override;

    virtual void deactivate(PanelActions *actions) override;

    Q_SLOT virtual void itemActionHandler(ItemAction action) override;

    Q_SLOT void selectionChangedHandler(const QItemSelection &selected, const QItemSelection &deselected);

    Q_SLOT void viewOrphanSourceAccounts();

    Q_SLOT void rowsInsertedHandler(const QModelIndex &parent, int first, int last);

    Q_SLOT void rowsMovedHandler(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);

    Q_SLOT void showClosedToggleHandler();

    Q_SLOT void sectionResizedHandler(int logicalIndex, int oldSize, int newSize);

    Q_SLOT void itemExpandedHandler(const QModelIndex &index);

private:
    class AccountsManagementPanelImpl;
    std::shared_ptr<AccountsManagementPanelImpl> impl;
};

#endif // ACCOUNTSMANAGEMENTPANEL_H
