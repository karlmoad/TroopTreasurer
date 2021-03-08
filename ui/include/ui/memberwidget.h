#ifndef MEMBERWIDGET_H
#define MEMBERWIDGET_H

#include <QWidget>
#include <memory>

class MemberWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MemberWidget(QWidget *parent = nullptr);
    ~MemberWidget();

private:
    class MemberWidgetImpl;
    std::shared_ptr<MemberWidgetImpl> impl;
};

#endif // MEMBERWIDGET_H
