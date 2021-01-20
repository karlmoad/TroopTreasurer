#ifndef FUNDSREGISTERPANEL_H
#define FUNDSREGISTERPANEL_H

#include <QWidget>

namespace Ui {
class FundsRegisterPanel;
}

class FundsRegisterPanel : public QWidget
{
    Q_OBJECT

public:
    explicit FundsRegisterPanel(QWidget *parent = nullptr);
    ~FundsRegisterPanel();

private:
    Ui::FundsRegisterPanel *ui;
};

#endif // FUNDSREGISTERPANEL_H
