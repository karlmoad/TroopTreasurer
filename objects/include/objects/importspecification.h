#ifndef TROOPTREASURER_IMPORTSPECIFICATION_H
#define TROOPTREASURER_IMPORTSPECIFICATION_H

#include <QString>
#include <QList>
#include <QMap>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <memory>
#include "objecterror.h"


class ImportSpecification
{
public:
    ImportSpecification(const QString& name, const QString& target);
    ImportSpecification(const QJsonObject& json);
    ~ImportSpecification();

    QString getName() const;
    QString getTarget() const;

    QList<QString> getFields() const;
    QString getFieldExpression(const QString& id) const;
    void setField(const QString& id, const QString& expression);
    void removeField(const QString& id);
    QJsonObject toJson() const;

private:
    class ImportSpecificationImpl;
    std::shared_ptr<ImportSpecificationImpl> impl;
};

class ImportSpecificationFactory
{
public:
    static QList<ImportSpecification> Load(const QString& filename);
    static void Save(const QList<ImportSpecification>& specs, const QString& filename);
};




#endif //TROOPTREASURER_IMPORTSPECIFICATION_H
