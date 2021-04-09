/*
 * TroopTreasurer
 * Author: Karl Moad
 * ---------------------------------
 * DataObject - base class representing
 * a database object with an underlying json construct
*/


#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "utility.h"
#include "dates.h"
#include "resultstatus.h"

class DataObject
{
public:
    virtual QString key() const = 0;
    virtual bool isNull() = 0;
    virtual QJsonObject json() const =0;
};

class DataObjectImpl
{
public:
    DataObjectImpl();
    explicit DataObjectImpl(const DataObjectImpl &other);
    explicit  DataObjectImpl(const QJsonObject& json);
    ~DataObjectImpl();

    QJsonValue getValue(const QString& key);
    void setValue(const QString& key, const QJsonValue& value);
    virtual QJsonObject json() const;
    virtual QString key() const;
    virtual bool isNull();

    DataObjectImpl& operator=(const DataObjectImpl& other);
    bool operator==(const DataObjectImpl &rhs) const;
    bool operator!=(const DataObjectImpl &rhs) const;

private:
    QString _key;
    QJsonObject _data;
};


#endif //DATAOBJECT_H
