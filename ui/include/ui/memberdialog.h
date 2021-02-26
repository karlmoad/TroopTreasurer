#ifndef MEMBERDIALOG_H
#define MEMBERDIALOG_H

#include <QDialog>
#include <memory>

class MemberDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MemberDialog(QWidget *parent = nullptr);
    ~MemberDialog();

private:
    class MemberDialogImpl;
    std::shared_ptr<MemberDialogImpl> impl;
};

#endif // MEMBERDIALOG_H
