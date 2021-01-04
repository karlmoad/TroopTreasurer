#ifndef TROOPTREASURER_EXPRESSION_H
#define TROOPTREASURER_EXPRESSION_H

#include <QList>
#include <QVariant>
#include "argument.h"

class Expression
{
public:
    explicit Expression(int numArgs);

    virtual ~Expression();
    void addArgument(Argument *arg);
    virtual QVariant execute(const QList<QString> &record) = 0;

protected:
    int _numArgz;
    QList<Argument*> _argz;
};


#endif //TROOPTREASURER_EXPRESSION_H
