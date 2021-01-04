#ifndef TROOPTREASURER_UUIDEXPRESSION_H
#define TROOPTREASURER_UUIDEXPRESSION_H

#include <QUuid>
#include <QDebug>
#include "expression.h"

class UUID: public Expression
{
public:
    UUID():Expression(0){};
    virtual ~UUID(){};

    virtual QVariant execute(const QList<QString> &record) override
    {
        return QUuid::createUuid().toString();
    }
};

#endif //TROOPTREASURER_UUIDEXPRESSION_H
