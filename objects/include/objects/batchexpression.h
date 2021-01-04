#ifndef TROOPTREASURER_BATCHEXPRESSION_H
#define TROOPTREASURER_BATCHEXPRESSION_H

#include <QUuid>
#include <QDebug>
#include "expression.h"

class BatchId: public Expression
{
public:
    BatchId():Expression(0), _id(QUuid::createUuid().toString()){};
    virtual ~BatchId(){};
    virtual QVariant execute(const QList<QString> &record) override
    {
        return _id;
    }
private:
    QString _id;
};


#endif //TROOPTREASURER_BATCHEXPRESSION_H
