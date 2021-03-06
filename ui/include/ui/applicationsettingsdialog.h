#ifndef APPLICATIONSETTINGSDIALOG_H
#define APPLICATIONSETTINGSDIALOG_H

#include <QDialog>
#include <memory>
#include "mainsettingspane.h"
#include "applicationsettings.h"
#include "ui.h"

class ApplicationSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    REM_COPY_MOVE(ApplicationSettingsDialog)
    explicit ApplicationSettingsDialog(QWidget *parent = nullptr);
    ~ApplicationSettingsDialog();

public slots:
    void onSaveClickHandler();
    void onCloseClickHandler();
    void onBackClickHandler();
    void onSettingPaneSelection(ApplicationSettingsType type);

private:
    class ApplicationSettingsDialogImpl;
    std::shared_ptr<ApplicationSettingsDialogImpl> impl;
};

#endif // APPLICATIONSETTINGSDIALOG_H
