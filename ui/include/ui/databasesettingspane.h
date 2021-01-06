/*
 * TroopTreasurer UI
 * Author: Karl Moad
 * ---------------------------------
 * DatabaseSettingsPane - Pane displayed within
 * the settings panel to collect and test values
 * required for the application database connection.
 * configuration and settings are pulled form the
 * static SettingsManager object by the settings main
 * panel and supplied to this pane's interface.
 */

#ifndef DATABASESETTINGSPANE_H
#define DATABASESETTINGSPANE_H

#include <QWidget>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <memory>
#include "applicationsettingspane.h"
#include "applicationsettings.h"

class DatabaseSettingsPane : public QWidget, public ApplicationSettingsPane
{
    Q_OBJECT
public:
    explicit DatabaseSettingsPane(QWidget *parent = nullptr);

    ~DatabaseSettingsPane();

    void resizeEvent(QResizeEvent *event) override;

    virtual ApplicationSettingsType settingsType() override;

    virtual void loadSettings(const QJsonObject& meta, const QJsonObject& settings) override;

    virtual QJsonObject settings() override;

    Q_SLOT void testConnectionSettings();

private:
    class DatabaseSettingsPaneImpl;
    std::shared_ptr<DatabaseSettingsPaneImpl> impl;
};

#endif // DATABASESETTINGSPANE_H