#ifndef FUNDSRECORDEDITDIALOG_H
#define FUNDSRECORDEDITDIALOG_H

#include <QDialog>
#include <memory>
#include "objects/fundsrecord.h"
#include "ui.h"

class FundsRecordEditDialog : public QDialog
{
    Q_OBJECT

public:
    REM_COPY_MOVE(FundsRecordEditDialog)
    explicit FundsRecordEditDialog(QWidget *parent = nullptr);
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
