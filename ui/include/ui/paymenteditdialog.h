#ifndef PAYMENTEDITDIALOG_H
#define PAYMENTEDITDIALOG_H

#include <QDialog>
#include <memory>
#include "objects/payment.h"
#include "ui.h"

class PaymentEditDialog : public QDialog
{
    Q_OBJECT
public:
    REM_COPY_MOVE(PaymentEditDialog)
    explicit PaymentEditDialog(QWidget *parent = nullptr);
    PaymentEditDialog(const PaymentEditDialog&) = delete;
    PaymentEditDialog(PaymentEditDialog&&) = delete;
    PaymentEditDialog& operator=(const PaymentEditDialog&) = delete;
    PaymentEditDialog& operator=(PaymentEditDialog&&) = delete;
    ~PaymentEditDialog();

    void setRecord(Transactions::Payment *payment, UI::Action action = UI::Action::NONE);


public slots:
    void okHandler();
    void cancelHandler();

private:
    class PaymentEditDialogImpl;
    std::shared_ptr<PaymentEditDialogImpl> impl;
};

#endif // PAYMENTEDITDIALOG_H
