#include "ui/applicationsettingsdialog.h"
#include "ui_applicationsettingsdialog.h"
#include "ui/databasesettingspane.h"

class ApplicationSettingsDialog::ApplicationSettingsDialogImpl
{
public:
    ApplicationSettingsDialogImpl(ApplicationSettingsDialog *pane): _pane(pane)
    {
        _ui = new Ui::ApplicationSettingsDialog();
        _ui->setupUi(_pane);
        init();
        connect(_ui->btnBack, &QPushButton::clicked, _pane, &ApplicationSettingsDialog::onBackClickHandler);
        connect(_ui->btnSave, &QPushButton::clicked, _pane, &ApplicationSettingsDialog::onSaveClickHandler);
        connect(_ui->btnClose, &QPushButton::clicked, _pane, &ApplicationSettingsDialog::onCloseClickHandler);
        connect(_main, &MainSettingsPane::settingsSectionSelected, _pane, &ApplicationSettingsDialog::onSettingPaneSelection);
    }

    ~ApplicationSettingsDialogImpl()
    {
        delete _ui;
    }

    void init()
    {
        enableBack(false);
        _main = new MainSettingsPane(_pane);
        int idx = _ui->panes->addWidget(_main);
        _ui->panes->setCurrentIndex(idx);
    }

    void enableBack(bool enable)
    {
        _ui->btnBack->setEnabled(enable);
        _ui->btnBack->setVisible(enable);
    }

    void showPane(ApplicationSettingsType type)
    {
        switch(type)
        {
            case ApplicationSettingsType::DATABASE:
            {
                enableBack(true);
                DatabaseSettingsPane *dbp = new DatabaseSettingsPane(_pane);
                int idx = _ui->panes->addWidget(dbp);
                _ui->panes->setCurrentIndex(idx);
            }
            default:
                break;
        }
    }

    void backClicked()
    {
        enableBack(false);
        QWidget *pane = _ui->panes->widget(1);
        _ui->panes->removeWidget(pane);
        _ui->panes->setCurrentIndex(0);
    }

private:
    ApplicationSettingsDialog *_pane;
    Ui::ApplicationSettingsDialog *_ui;
    MainSettingsPane *_main;
};

ApplicationSettingsDialog::ApplicationSettingsDialog(QWidget *parent) :
    QDialog(parent),
    impl(new ApplicationSettingsDialogImpl(this))
{

}

ApplicationSettingsDialog::~ApplicationSettingsDialog()
{
    delete impl;
}

void ApplicationSettingsDialog::onSaveClickHandler()
{
    this->close();
}

void ApplicationSettingsDialog::onCloseClickHandler()
{
    this->close();
}

void ApplicationSettingsDialog::onBackClickHandler()
{
    impl->backClicked();
}

void ApplicationSettingsDialog::onSettingPaneSelection(ApplicationSettingsType type)
{
    impl->showPane(type);
}
