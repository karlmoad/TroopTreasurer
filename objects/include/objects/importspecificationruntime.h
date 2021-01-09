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
#include <QAbstractTableModel>
#include <memory>
#include "expression.h"
#include "argument.h"
#include "expressionfactory.h"
#include "objecterror.h"
#include "importspecification.h"

class ImportSpecificationRuntime
{
public:
    explicit ImportSpecificationRuntime(const ImportSpecification& spec);
    virtual ~ImportSpecificationRuntime();

    bool compile();
    void setModel(QAbstractTableModel *model);
    QJsonObject process(int row);

private:
    class ImportSpecificationRuntimeImpl;
    std::shared_ptr<ImportSpecificationRuntimeImpl> impl;
};


#endif //TROOPTREASURER_IMPORTSPECIFICATIONRUNTIME_H
