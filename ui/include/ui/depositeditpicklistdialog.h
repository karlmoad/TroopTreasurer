#ifndef DESPOITEDITPICKLISTDIALOG_H
#define DESPOITEDITPICKLISTDIALOG_H

#include <QDialog>
#include <memory>
#include <QList>
#include "objects/fundsrecord.h"
#include "ui.h"

class DepositEditPicklistDialog : public QDialog
{
    Q_OBJECT

public:
    REM_COPY_MOVE(DepositEditPicklistDialog)
    explicit DepositEditPicklistDialog(QWidget *parent = nullptr);
    ~DepositEditPicklistDialog();

    QList<Transactions::FundsRecord> getSelected();

public slots:
    void okHandler();
    void cancelHandler();

private:
    class DepositEditPicklistDialogImpl;
    std::shared_ptr<DepositEditPicklistDialogImpl> impl;
};

#endif // DESPOITEDITPICKLISTDIALOG_H
