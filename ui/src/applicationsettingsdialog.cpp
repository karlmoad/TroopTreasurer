#include <QMap>
#include <QList>
#include <QJsonObject>
#include <QMessageBox>
#include "ui/applicationsettingsdialog.h"
#include "ui_applicationsettingsdialog.h"
#include "ui/databasesettingspane.h"
#include "objects/settingsmanager.h"
#include "objects/objecterror.h"

class ApplicationSettingsDialog::ApplicationSettingsDialogImpl
{
public:
    ApplicationSettingsDialogImpl(ApplicationSettingsDialog *pane): _pane(pane)
    {
        _ui = new Ui::ApplicationSettingsDialog();
        _ui->setupUi(_pane);
        _main = new MainSettingsPane(_pane);
        _ui->panes->addWidget(_main);
        _ui->panes->setCurrentIndex(0);
        settingsManager = SettingsManager::getInstance();
        init();
        connect(_ui->btnBack, &QPushButton::clicked, _pane, &ApplicationSettingsDialog::onBackClickHandler);
        connect(_ui->btnSave, &QPushButton::clicked, _pane, &ApplicationSettingsDialog::onSaveClickHandler);
        connect(_ui->btnClose, &QPushButton::clicked, _pane, &ApplicationSettingsDialog::onCloseClickHandler);
        connect(_main, &MainSettingsPane::settingsSectionSelected, _pane, &ApplicationSettingsDialog::onSettingPaneSelection, Qt::UniqueConnection);
    }

    ~ApplicationSettingsDialogImpl()
    {
        delete _ui;
    }

    void init()
    {
        enableSubPaneButtons(false);
        setWindowTitle(defaultWindowTitle);
    }

    void enableSubPaneButtons(bool enable)
    {
        _ui->btnBack->setEnabled(enable);
        _ui->btnBack->setVisible(enable);
        _ui->btnSave->setEnabled(enable);
        _ui->btnSave->setVisible(enable);
        _ui->btnClose->setEnabled(!enable);
        _ui->btnClose->setVisible(!enable);
    }

    void showPane(ApplicationSettingsType type)
    {
        if(_ui->panes->count() == 1) // Only add a new pane if no others exist
        {
            switch (type)
            {
                case ApplicationSettingsType::DATABASE:
                {
                    enableSubPaneButtons(true);
                    DatabaseSettingsPane *dbp = new DatabaseSettingsPane(_pane);
                    dbp->loadSettings(settingsManager->getConfigurationSectionMetadata(
                            ApplicationSettingsUtility::ApplicationSettingTypeToString(
                                    ApplicationSettingsType::DATABASE)),
                                      settingsManager->getSettingsSegment(
                                              ApplicationSettingsUtility::ApplicationSettingTypeToString(
                                                      ApplicationSettingsType::DATABASE)));
                    int idx = _ui->panes->addWidget(dbp);
                    _ui->panes->setCurrentIndex(idx);
                    setWindowTitle("Database Settings");
                }
                default:
                    break;
            }
        }
    }

    void backClicked()
    {
        if(_ui->panes->count() > 1)
        {
            enableSubPaneButtons(false);
            QWidget *widget = _ui->panes->widget(1);
            _ui->panes->removeWidget(widget);
            _ui->panes->setCurrentIndex(0);
            setWindowTitle(defaultWindowTitle);
        }
    }

    void save()
    {
        if(_ui->panes->count() > 1)
        {
            QWidget *widget = _ui->panes->widget(1);
            ApplicationSettingsPane *pane = dynamic_cast<ApplicationSettingsPane*>(widget);
            if(pane)
            {
                //set pane settings values into settingsManager context
                settingsManager->setSettingsSegment(pane->settings(),ApplicationSettingsUtility::ApplicationSettingTypeToString(pane->settingsType()));
                QString msg;
                //save SettingsManager context
                if (!settingsManager->saveSettings(&msg))
                {
                    QMessageBox::critical(_pane, "Error", msg);
                }
            }
            else
            {
                QMessageBox::critical(_pane,"Error","Error unable to save settings");
            }
        }
        else
        {
            QMessageBox::critical(_pane,"Error","Error unable to save settings, invalid settings pane state");
        }
    }

    void setWindowTitle(const QString &title)
    {
        _pane->setWindowTitle(title);
    }

private:
    ApplicationSettingsDialog *_pane;
    Ui::ApplicationSettingsDialog *_ui;
    MainSettingsPane *_main;
    SettingsManager *settingsManager;
    const QString defaultWindowTitle = "Application Settings";
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
    impl->save();
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
