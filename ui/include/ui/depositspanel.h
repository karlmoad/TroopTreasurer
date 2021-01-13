#ifndef DEPOSITSPANEL_H
#define DEPOSITSPANEL_H

#include <QWidget>
#include <memory>

class DepositsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit DepositsPanel(QWidget *parent = nullptr);
    ~DepositsPanel();

private:
    class DepositsPanelImpl;
    std::shared_ptr<DepositsPanelImpl> impl;
};

#endif // DEPOSITSPANEL_H
