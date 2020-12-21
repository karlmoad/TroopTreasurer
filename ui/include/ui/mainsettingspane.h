#ifndef MANSETTINGSPANE_H
#define MANSETTINGSPANE_H

#include <QWidget>
#include <QListWidgetItem>
#include "applicationsettings.h"

class MainSettingsPane : public QWidget
{
    Q_OBJECT

public:
    explicit MainSettingsPane(QWidget *parent = nullptr);
    ~MainSettingsPane();

    Q_SLOT void paneListItemClicked(QListWidgetItem *item);

signals:
    void settingsSectionSelected(ApplicationSettingsType type);

private:
    class MainSettingsPaneImpl;
    MainSettingsPaneImpl *impl;

};

#endif // MANSETTINGSPANE_H
