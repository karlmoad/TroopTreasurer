#ifndef TROOPTREASURER_EXPRESSIONFACTORY_H
#define TROOPTREASURER_EXPRESSIONFACTORY_H

#include <QString>
#include <QVariant>
#include <QQueue>
#include <QDebug>
#include <QMap>
#include "expression.h"
#include "batchexpression.h"
#include "uuidexpression.h"
#include "hashingexpression.h"
#include "valueexpression.h"
#include "objecterror.h"

class ExpressionFactory
{
public:
    static Expression* CompileExpression(const QString& expression, const QList<QString>& sourceColumns);

private:
    const static QMap<QString,int> _funcMap;
    static Expression* FindExpression(const QString& name, int argCount=0);
    static Expression* CreateRecordValueExpression(int ref=-1);
    static Expression* CreateValueOfExpression(QVariant value);
};



#endif //TROOPTREASURER_EXPRESSIONFACTORY_H
