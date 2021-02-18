#ifndef DEPOSITEDITDIALOG_H
#define DEPOSITEDITDIALOG_H

#include <QDialog>
#include <memory>
#include "objects/deposit.h"
#include "objects/fundsrecord.h"
#include "ui.h"

class DepositEditDialog : public QDialog
{
    Q_OBJECT

public:
    REM_COPY_MOVE(DepositEditDialog)
    explicit DepositEditDialog(QWidget *parent = nullptr);
    ~DepositEditDialog();
    DepositEditDialog(const DepositEditDialog&) = delete;
    DepositEditDialog(DepositEditDialog&&) = delete;
    DepositEditDialog& operator=(const DepositEditDialog&) = delete;
    DepositEditDialog& operator=(DepositEditDialog&&) = delete;

    void setDeposit(std::shared_ptr<Transactions::Deposit> deposit, UI::Action action= UI::Action::NONE);

public slots:
    void okHandler();
    void cancelHandler();
    void addHandler();
    void remHandler();

private:
    class DepositEditDialogImpl;
    std::shared_ptr<DepositEditDialogImpl> impl;
};

#endif // DEPOSITEDITDIALOG_H
