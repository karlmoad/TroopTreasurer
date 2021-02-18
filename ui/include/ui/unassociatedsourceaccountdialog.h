#ifndef UNASSOCIATEDSOURCEACCOUNTDIALOG_H
#define UNASSOCIATEDSOURCEACCOUNTDIALOG_H

#include <QDialog>
#include <QList>
#include <QJsonObject>
#include <QJsonValue>
#include <memory>
#include "ui.h"

class UnassociatedSourceAccountDialog : public QDialog
{
    Q_OBJECT
public:
    REM_COPY_MOVE(UnassociatedSourceAccountDialog)
    explicit UnassociatedSourceAccountDialog(QWidget *parent = nullptr);
    ~UnassociatedSourceAccountDialog();

    void setData(const QList<QJsonObject> &data);
    QJsonObject getSelected();

public slots:
    void closeHandler();
    void createHandler();

private:
    class UnassociatedSourceAccountDialogImpl;
    std::shared_ptr<UnassociatedSourceAccountDialogImpl> impl;
};

#endif // UNASSOCIATEDSOURCEACCOUNTDIALOG_H
