#ifndef TROOPTREASURER_ARGUMENT_H
#define TROOPTREASURER_ARGUMENT_H


#include <QObject>
#include <QVector>
#include <QVariant>
#include <QDebug>


class Expression;

class Argument
{
public:
    explicit Argument(Expression *expression);
    virtual ~Argument();

    QVariant getValue(const QList<QString> &record);
private:
    Expression *_d;
};



#endif //TROOPTREASURER_ARGUMENT_H
