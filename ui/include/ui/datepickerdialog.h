#ifndef DATEPICKERDIALOG_H
#define DATEPICKERDIALOG_H

#include <QDialog>
#include <QDate>
#include <memory>
#include "ui.h"

class DatePickerDialog : public QDialog
{
    Q_OBJECT

public:
    REM_COPY_MOVE(DatePickerDialog)
    explicit DatePickerDialog(QWidget *parent = nullptr);
    ~DatePickerDialog();

    QDate selected();

public slots:
    void okHandler();
    void cancelHandler();

private:
    class DatePickerDialogImpl;
    std::shared_ptr<DatePickerDialogImpl> impl;
};

#endif // DATEPICKERDIALOG_H
