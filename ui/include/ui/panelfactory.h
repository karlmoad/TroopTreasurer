//
// Created by Karl Moad on 12/18/20.
//

#ifndef TROOPTREASURER_PANELFACTORY_H
#define TROOPTREASURER_PANELFACTORY_H

#include <QtWidgets>
#include "panelwindow.h"

enum class Panel
{
    FUNDS_MANAGEMENT=0,
    IMPORT_TEMPLATE_EDITOR
};

class PanelFactory
{
public:
    static bool PanelSupportsMultipleInstances(Panel panel);
    static PanelWindow* CreateNewPanel(Panel panel, QWidget *parent=nullptr);
};

#endif //TROOPTREASURER_PANELFACTORY_H
