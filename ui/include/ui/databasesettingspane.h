#ifndef DATABASESETTINGSPANE_H
#define DATABASESETTINGSPANE_H

#include <QWidget>
#include "applicationsettingspane.h"
#include "applicationsettings.h"

namespace Ui {
class DatabaseSettingsPane;
}

class DatabaseSettingsPane : public QWidget, public ApplicationSettingsPane
{
    Q_OBJECT
public:
    virtual void loadConfiguration(const QJsonObject& config) override;

public:
    explicit DatabaseSettingsPane(QWidget *parent = nullptr);

    ~DatabaseSettingsPane();

    virtual ApplicationSettingsType settingsType() override;

    virtual void loadSettings(const QJsonObject &settings) override;

    virtual QJsonObject settings() override;

    virtual void save() override;

private:
    Ui::DatabaseSettingsPane *ui;
};

#endif // DATABASESETTINGSPANE_H
