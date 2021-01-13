#include "ui/depositspanel.h"
#include "ui_depositspanel.h"

class DepositsPanel::DepositsPanelImpl
{
public:
    DepositsPanelImpl(DepositsPanel *panel): _panel(panel), _ui(new Ui::DepositsPanel)
    {
        _ui->setupUi(_panel);
    }

    ~DepositsPanelImpl()
    {
        delete _ui;
    }


private:
    DepositsPanel *_panel;
    Ui::DepositsPanel *_ui;
};

DepositsPanel::DepositsPanel(QWidget *parent) :
    QWidget(parent),
    impl(new DepositsPanelImpl(this)){}

DepositsPanel::~DepositsPanel(){}
