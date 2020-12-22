#include "ui/databasesettingspane.h"
#include "ui_databasesettingspane.h"

DatabaseSettingsPane::DatabaseSettingsPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DatabaseSettingsPane)
{
    ui->setupUi(this);
}

DatabaseSettingsPane::~DatabaseSettingsPane()
{
    delete ui;
}

ApplicationSettingsType DatabaseSettingsPane::settingsType()
{
    return ApplicationSettingsType::DATABASE;
}

void DatabaseSettingsPane::loadSettings(const QJsonObject &settings)
{

}

QJsonObject DatabaseSettingsPane::settings()
{
    return QJsonObject();
}

void DatabaseSettingsPane::save()
{

}

void DatabaseSettingsPane::loadConfiguration(const QJsonObject &config)
{

}
