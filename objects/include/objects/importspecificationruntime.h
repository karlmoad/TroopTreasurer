#ifndef TROOPTREASURER_IMPORTSPECIFICATIONRUNTIME_H
#define TROOPTREASURER_IMPORTSPECIFICATIONRUNTIME_H

#include <QList>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QDebug>
#include <memory>
#include "expression.h"
#include "argument.h"
#include "expressionfactory.h"
#include "objecterror.h"
#include "importspecification.h"

class ImportSpecificationRuntime
{
public:
    explicit ImportSpecificationRuntime(const ImportSpecification& spec, const QList<QString>& sourceColumns);
    virtual ~ImportSpecificationRuntime();

    QMap<QString,QVariant> process(const QList<QString> &record);

private:
    class ImportSpecificationRuntimeImpl;
    std::shared_ptr<ImportSpecificationRuntimeImpl> impl;
};


#endif //TROOPTREASURER_IMPORTSPECIFICATIONRUNTIME_H
