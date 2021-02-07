//
// Created by Karl Moad on 12/18/20.
//

#include "ui/panelfactory.h"
#include "ui/importtemplatepanel.h"
#include "ui/paymentspanel.h"
#include "ui/fundsregisterpanel.h"
#include "ui/depositspanel.h"
#include "ui/accountsmanagementpanel.h"

bool PanelFactory::PanelSupportsMultipleInstances(Panel panel)
{
    switch(panel)
    {
        default:
            return false;
    }
}

PanelWindow *PanelFactory::CreateNewPanel(Panel panel, QWidget *parent)
{
    switch(panel)
    {
        case Panel::IMPORT_TEMPLATE_EDITOR:
            return (PanelWindow*)new ImportTemplatePanel(parent);
        case Panel::PAYMENTS:
            return (PanelWindow*)new PaymentsPanel(parent);
        case Panel::FUNDS:
            return (PanelWindow*)new FundsRegisterPanel(parent);
        case Panel::DEPOSITS:
            return (PanelWindow*)new DepositsPanel(parent);
        case Panel::ACCOUNTSMGMT:
            return (PanelWindow*)new AccountsManagementPanel(parent);
        default:
            return nullptr;
    }
}
