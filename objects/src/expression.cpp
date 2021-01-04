#include "objects/expression.h"

void Expression::addArgument(Argument *arg)
{
    _argz.append(arg);
}

Expression::Expression(int numArgs)
{
    _numArgz = numArgs;
}

Expression::~Expression()
{
    qDeleteAll(_argz);
}
