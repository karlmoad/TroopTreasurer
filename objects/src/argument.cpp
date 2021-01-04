#include "objects/argument.h"
#include "objects/expression.h"

Argument::Argument(Expression *expression): _d(nullptr)
{
    if(expression != nullptr)
    {
        _d = expression;
    }
}

Argument::~Argument()
{
    delete(_d);
}

QVariant Argument::getValue(const QList<QString> &record)
{
    if(this->_d != nullptr)
    {
        return _d->execute(record);
    }
    return QVariant();
}

