//
// Created by Karl Moad on 12/28/20.
//

#ifndef TROOPTREASURER_IMPORTTEMPLATETESTDIALOG_H
#define TROOPTREASURER_IMPORTTEMPLATETESTDIALOG_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class importtemplatetestdialog;
}
QT_END_NAMESPACE

class importtemplatetestdialog : public QWidget
{
Q_OBJECT

public:
    explicit importtemplatetestdialog(QWidget *parent = nullptr);

    ~importtemplatetestdialog() override;

private:
    Ui::importtemplatetestdialog *ui;
};

#endif //TROOPTREASURER_IMPORTTEMPLATETESTDIALOG_H
