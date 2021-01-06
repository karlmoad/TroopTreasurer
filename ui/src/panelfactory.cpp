//
// Created by Karl Moad on 12/18/20.
//

#include "ui/panelfactory.h"
#include "ui/importtemplatepanel.h"

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
        case Panel::UNDEFINED:
            return nullptr;
        case Panel::IMPORT_TEMPLATE_EDITOR:
            return (PanelWindow*)new ImportTemplatePanel(parent);
        default:
            return nullptr;
    }
}
