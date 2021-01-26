#ifndef DEPOSITEDITDIALOG_H
#define DEPOSITEDITDIALOG_H

#include <QDialog>

namespace Ui {
class DepositEditDialog;
}

class DepositEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DepositEditDialog(QWidget *parent = nullptr);
    ~DepositEditDialog();

private:
    Ui::DepositEditDialog *ui;
};

#endif // DEPOSITEDITDIALOG_H
