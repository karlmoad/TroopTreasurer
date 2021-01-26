#ifndef DESPOITEDITPICKLISTDIALOG_H
#define DESPOITEDITPICKLISTDIALOG_H

#include <QDialog>

namespace Ui {
class DespoitEditPicklistDialog;
}

class DespoitEditPicklistDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DespoitEditPicklistDialog(QWidget *parent = nullptr);
    ~DespoitEditPicklistDialog();

private:
    Ui::DespoitEditPicklistDialog *ui;
};

#endif // DESPOITEDITPICKLISTDIALOG_H
