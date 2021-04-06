#ifndef DATAOBJECTCONTROLLER_H
#define DATAOBJECTCONTROLLER_H

#include <QList>
#include <QMap>
#include <QVariant>
#include <QJsonObject>
#include "resultstatus.h"
#include "dataaccessobject.h"

template <typename T>
class DataObjectController
{
public:
    virtual ResultStatus load(const QMap<QString, QVariant>& args)=0;
    virtual void setData(const QList<T>& objects)=0;
    virtual std::shared_ptr<DataAccessObject<T>> dataAccessObject()=0;
    virtual int count() = 0;
    virtual int indexOf(const QString& key)=0;
    virtual const T& getObject(int index) = 0;
    virtual QJsonObject getJson(int index) =0;
};

template <typename  T>
class DataObjectControllerAddBinding
{
public:
    virtual ResultStatus add(const T&) = 0;
};

template <typename  T>
class DataObjectControllerUpdateBinding
{
public:
    virtual ResultStatus update(const T&) = 0;
};

template <typename  T>
class DataObjectControllerRemoveBinding
{
public:
    virtual ResultStatus remove(int) = 0;
}


#endif //DATAOBJECTCONTROLLER_H
