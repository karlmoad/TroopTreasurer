#ifndef HOUSEHOLDDIALOG_H
#define HOUSEHOLDDIALOG_H

#include <QDialog>
#include <memory>

class HouseholdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HouseholdDialog(QWidget *parent = nullptr);
    ~HouseholdDialog();

private:
    class HouseholdDialogImpl;
    std::shared_ptr<HouseholdDialogImpl> impl;
};

#endif // HOUSEHOLDDIALOG_H
