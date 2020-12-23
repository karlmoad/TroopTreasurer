#include <QMap>
#include <QList>
#include <QJsonObject>
#include <QMessageBox>
#include "ui/applicationsettingsdialog.h"
#include "ui_applicationsettingsdialog.h"
#include "ui/databasesettingspane.h"
#include "objects/settingsmanager.h"
#include "objects/objecterror.h"
#include <QDebug>

class ApplicationSettingsDialog::ApplicationSettingsDialogImpl
{
public:
    ApplicationSettingsDialogImpl(ApplicationSettingsDialog *pane): _pane(pane)
    {
        _ui = new Ui::ApplicationSettingsDialog();
        _ui->setupUi(_pane);
        settingsManager = SettingsManager::getInstance();
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
        enableSubPaneButtons(false);
        _main = new MainSettingsPane(_pane);
        int idx = _ui->panes->addWidget(_main);
        _ui->panes->setCurrentIndex(idx);
        setWindowTitle(defaultWindowTitle);
    }

    void enableSubPaneButtons(bool enable)
    {
        _ui->btnBack->setEnabled(enable);
        _ui->btnBack->setVisible(enable);
        _ui->btnSave->setEnabled(enable);
    }

    void showPane(ApplicationSettingsType type)
    {
        switch(type)
        {
            case ApplicationSettingsType::DATABASE:
            {
                enableSubPaneButtons(true);
                DatabaseSettingsPane *dbp = new DatabaseSettingsPane(_pane);
                dbp->loadSettings(settingsManager->getConfigurationSectionMetadata(ApplicationSettingsUtility::ApplicationSettingTypeToString(ApplicationSettingsType::DATABASE)),
                                  settingsManager->getSettingsSegment(ApplicationSettingsUtility::ApplicationSettingTypeToString(ApplicationSettingsType::DATABASE)));
                int idx = _ui->panes->addWidget(dbp);
                _ui->panes->setCurrentIndex(idx);
                setWindowTitle("Database Settings");
            }
            default:
                break;
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
        bool proceed = true;
        if(_ui->panes->count() > 1) // make sure a sub-pane is currently active, get its content and push into change list
        {
            proceed = syncCurrentChanges();
        }

        if(proceed)
        {
            QList<ApplicationSettingsType> keys = _changed.keys();
            //set all changed segments in the settings manager
            for (int i = 0; i < keys.count(); i++)
            {
                settingsManager->setSettingsSegment(_changed[keys[i]],
                                                    ApplicationSettingsUtility::ApplicationSettingTypeToString(
                                                            keys[i]));
            }

            QString msg;
            if (!settingsManager->saveSettings(&msg))
            {
                QMessageBox::critical(_pane, "Error", msg);
            }
        }
        else
        {
            QMessageBox::critical(_pane,"Error","Error unable to synchronize settings");
        }
    }

    void setWindowTitle(const QString &title)
    {
        _pane->setWindowTitle(title);
    }


private:
    bool syncCurrentChanges()
    {
        if(_ui->panes->count() > 1)
        {
            QWidget *widget = _ui->panes->widget(1);
            ApplicationSettingsPane *pane = dynamic_cast<ApplicationSettingsPane*>(widget);
            if(pane)
            {
                mergeChange(pane->settingsType(),pane->settings());
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    void mergeChange(ApplicationSettingsType type, QJsonObject data)
    {
        if(_changed.contains(type))
        {
            _changed[type] = data;
        }
        else
        {
            _changed.insert(type, data);
        }
    }

private:
    ApplicationSettingsDialog *_pane;
    Ui::ApplicationSettingsDialog *_ui;
    MainSettingsPane *_main;
    SettingsManager *settingsManager;
    QMap<ApplicationSettingsType, QJsonObject> _changed;
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
