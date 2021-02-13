#ifndef DATEPICKERDIALOG_H
#define DATEPICKERDIALOG_H

#include <QDialog>
#include <QDate>
#include <memory>

class DatePickerDialog : public QDialog
{
    Q_OBJECT

public:
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
