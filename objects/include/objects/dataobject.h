/*
 * TroopTreasurer
 * Author: Karl Moad
 * ---------------------------------
 * DataObject - base class representing
 * a database object with an underlying json construct
*/


#ifndef TROOPTREASURER_DATAOBJECT_H
#define TROOPTREASURER_DATAOBJECT_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "utility.h"
#include "dates.h"

class DataObject
{
public:
    DataObject();
    explicit DataObject(const DataObject &other);
    ~DataObject();

    QJsonValue getValue(const QString& key);
    void setValue(const QString& key, const QJsonValue& value);
    const QJsonObject& json();

protected:
    explicit  DataObject(const QJsonObject& json);

private:
    QJsonObject _data;
};


#endif //TROOPTREASURER_DATAOBJECT_H
