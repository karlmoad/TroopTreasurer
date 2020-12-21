#ifndef IMPORTTEMPLATEPANEL_H
#define IMPORTTEMPLATEPANEL_H

#include <QWidget>
#include "itemstate.h"
#include "panelwindow.h"

namespace Ui {
class ImportTemplatePanel;
}

class ImportTemplatePanel : public PanelWindow
{
    Q_OBJECT
public:
    explicit ImportTemplatePanel(QWidget *parent = nullptr);
    ~ImportTemplatePanel();

    virtual ItemState getCurrentState() const override;

    virtual const QString &panelMenuText() const override;

    virtual bool hasMenu() const override;

    virtual void activate() override;

    virtual bool hasToolbarItems() const override;

    virtual const QList<QAction *>& menuItems() const override;

    virtual const QList<QAction *>& toolbarItems() const override;

    virtual QString panelName() const override;

    Q_SLOT virtual void itemActionHandler(ItemAction action) override;

    Q_SLOT void actionTestHandler();

    Q_SLOT void actionValidateHandler();

    Q_SLOT void sendUpdateButtonHandler();

private:
    class ImportTemplatePanelImpl;
    Ui::ImportTemplatePanel *ui;
    ImportTemplatePanelImpl *impl;


private:
    void itemActionNotifyChange(ItemState change);
    void sendCurrentSelectionState();


};

#endif // IMPORTTEMPLATEPANEL_H
