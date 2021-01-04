#ifndef TROOPTREASURER_HASHINGEXPRESSION_H
#define TROOPTREASURER_HASHINGEXPRESSION_H

#include <QVariant>
#include <QCryptographicHash>
#include <QDebug>
#include "expression.h"

class Hash256: public Expression
{
public:
    Hash256():Expression(1){};
    virtual ~Hash256(){};

    virtual QVariant execute(const QList<QString> &record) override
    {
        if(_numArgz == 1 && _argz.size() >= _numArgz)
        {
            QVariant v = _argz[0]->getValue(record);
            if(!v.isNull())
            {
                return QString(QCryptographicHash::hash(v.toString().toUtf8(),QCryptographicHash::Sha256).toHex());
            }
        }
        return QVariant();
    }
};


#endif //TROOPTREASURER_HASHINGEXPRESSION_H
