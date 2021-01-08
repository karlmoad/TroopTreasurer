#ifndef TROOPTREASURER_SCHEMA_H
#define TROOPTREASURER_SCHEMA_H

#include <QString>
#include <QList>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <memory>

class Schema
{
public:
    Schema();
    ~Schema();

    QString getName() const;
    QString getDisplay() const;
    QString getTable() const;
    bool isImportSupported() const;
    bool isDupCheckSupported() const;
    bool isTruncateSupported() const;
    QMap<QString, QJsonObject> getFields() const;
    QJsonObject toJson() const;

    static QMap<QString, Schema> load(const QString& filepath);
    static Schema loadSchema(const QString& schemaId, const QString& filepath);
    static Schema fromJson(const QJsonObject& json);
private:
    class SchemaImpl;
    std::shared_ptr<SchemaImpl> impl;
};


#endif //TROOPTREASURER_SCHEMA_H
