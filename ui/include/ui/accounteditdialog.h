#ifndef ACCOUNTEDITDIALOG_H
#define ACCOUNTEDITDIALOG_H

#include <QDialog>
#include <memory>
#include "ui/itemstate.h"
#include "objects/accountsmodel.h"
#include "ui.h"

class AccountEditDialog : public QDialog
{
    Q_OBJECT

public:
    REM_COPY_MOVE(AccountEditDialog)
    explicit AccountEditDialog(AccountsModel* model, ItemAction action ,QWidget *parent = nullptr);
    ~AccountEditDialog();

    void setContextItem(const QModelIndex &index);
    void setContextData(const QJsonObject& json);
    void load();

public slots:
    void okHandler();
    void cancelHandler();
    void parentSelectHandler();
    void parentResetHandler();

private:
    class AccountEditDialogImpl;
    std::shared_ptr<AccountEditDialogImpl> impl;
};

#endif // ACCOUNTEDITDIALOG_H
