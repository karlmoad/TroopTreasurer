#include "ui/datepickerdialog.h"
#include "ui_datepickerdialog.h"

class DatePickerDialog::DatePickerDialogImpl
{
public:
    DatePickerDialogImpl(DatePickerDialog *dialog):
                        _dialog(dialog),
                        _ui(new Ui::DatePickerDialog),
                        _selected(QDate::currentDate())
    {
        _ui->setupUi(_dialog);
        _ui->dateSelect->setSelectedDate(_selected);
        connect(_ui->btnOk, &QPushButton::clicked, _dialog, &DatePickerDialog::okHandler);
        connect(_ui->btnCancel, &QPushButton::clicked, _dialog, &DatePickerDialog::cancelHandler);
    }

    virtual ~DatePickerDialogImpl()
    {
        delete _ui;
    }

    QDate selected()
    {
        return _selected;
    }

    void setSelected()
    {
        this->_selected = _ui->dateSelect->selectedDate();
    }

private:
    DatePickerDialog *_dialog;
    Ui::DatePickerDialog *_ui;
    QDate _selected;
};

DatePickerDialog::DatePickerDialog(QWidget *parent) :
    QDialog(parent),
    impl(new DatePickerDialogImpl(this))
{

}

DatePickerDialog::~DatePickerDialog()
{}

QDate DatePickerDialog::selected()
{
    return impl->selected();
}

void DatePickerDialog::okHandler()
{
    impl->setSelected();
    this->done(QDialog::Accepted);
    this->close();
}

void DatePickerDialog::cancelHandler()
{
    this->done(QDialog::Rejected);
    this->close();
}
