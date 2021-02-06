#ifndef ACCOUNTSMANAGEMENTPANEL_H
#define ACCOUNTSMANAGEMENTPANEL_H

#include <QWidget>

namespace Ui {
class accountsmanagementpanel;
}

class accountsmanagementpanel : public QWidget
{
    Q_OBJECT

public:
    explicit accountsmanagementpanel(QWidget *parent = nullptr);
    ~accountsmanagementpanel();

private:
    Ui::accountsmanagementpanel *ui;
};

#endif // ACCOUNTSMANAGEMENTPANEL_H
