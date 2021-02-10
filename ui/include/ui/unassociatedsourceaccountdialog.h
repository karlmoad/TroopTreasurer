#ifndef UNASSOCIATEDSOURCEACCOUNTDIALOG_H
#define UNASSOCIATEDSOURCEACCOUNTDIALOG_H

#include <QDialog>
#include <QList>
#include <QJsonObject>
#include <QJsonValue>
#include <memory>

class UnassociatedSourceAccountDialog : public QDialog
{
    Q_OBJECT
public:
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
