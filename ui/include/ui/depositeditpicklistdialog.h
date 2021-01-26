#ifndef DESPOITEDITPICKLISTDIALOG_H
#define DESPOITEDITPICKLISTDIALOG_H

#include <QDialog>
#include <memory>
class DepositEditPicklistDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DepositEditPicklistDialog(QWidget *parent = nullptr);
    ~DepositEditPicklistDialog();

private:
    class DepositEditPicklistDialogImpl;
    std::shared_ptr<DepositEditPicklistDialogImpl> impl;
};

#endif // DESPOITEDITPICKLISTDIALOG_H
