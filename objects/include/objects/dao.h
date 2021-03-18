#ifndef TROOPTREASURER_DAO_H
#define TROOPTREASURER_DAO_H

#include <QModelIndex>
#include <QObject>
#include <QString>
#include <QList>
#include <QJsonObject>
#include "resultstatus.h"

template <typename T>
class DataAccessObject
{
public:
    virtual bool isHierarchical()=0;
    virtual int count()=0;
    virtual int indexOf(const QString& key) =0;
    virtual ResultStatus loadObjects(const QMap<QString,QString> &params)=0;
    virtual void loadObjects(const QList<T>& items)=0;
    virtual T getObject(const QString& key)=0;
    virtual T getObject(int index)=0;
    virtual QJsonObject getJson(const QString& key)=0;
    virtual QJsonObject getJson(int index)=0;

private:
    friend class DataObjectModel;
    virtual ResultStatus addObject(const T& object)=0;
    virtual ResultStatus updateObject(const T& object)=0;
    virtual ResultStatus removeObject(int index)=0;
};
#endif //TROOPTREASURER_DAO_H
