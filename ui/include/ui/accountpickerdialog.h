#ifndef ACCOUNTPICKERDIALOG_H
#define ACCOUNTPICKERDIALOG_H

#include <QDialog>
#include <memory>
#include "objects/accountsmodel.h"
#include "ui.h"

class AccountPickerDialog : public QDialog
{
    Q_OBJECT

public:
    REM_COPY_MOVE(AccountPickerDialog)
    explicit AccountPickerDialog(AccountsModel *model, QWidget *parent = nullptr);
    ~AccountPickerDialog();

    void setContextItem(const QModelIndex &index);
    QModelIndex getSelected() const;

public slots:
    void okHandler();
    void cancelHandler();

private:
    class AccountPickerDialogImpl;
    std::shared_ptr<AccountPickerDialogImpl> impl;
};

#endif // ACCOUNTPICKERDIALOG_H
