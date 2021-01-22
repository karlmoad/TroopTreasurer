#ifndef FUNDSRECORDEDITDIALOG_H
#define FUNDSRECORDEDITDIALOG_H

#include <QDialog>
#include <memory>
#include "objects/fundsrecord.h"
#include "ui/ui.h"

class FundsRecordEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FundsRecordEditDialog(QWidget *parent = nullptr);
    FundsRecordEditDialog(const FundsRecordEditDialog&) = delete;
    FundsRecordEditDialog(FundsRecordEditDialog&&) = delete;
    FundsRecordEditDialog& operator=(const FundsRecordEditDialog&) = delete;
    FundsRecordEditDialog& operator=(FundsRecordEditDialog&&) = delete;
    ~FundsRecordEditDialog();

    void setRecord(Transactions::FundsRecord *record, UI::Action action = UI::Action::NONE);

public slots:
    void okHandler();
    void cancelHandler();

private:
    class FundsRecordEditDialogImpl;
    std::shared_ptr<FundsRecordEditDialogImpl> impl;
};

#endif // FUNDSRECORDEDITDIALOG_H
