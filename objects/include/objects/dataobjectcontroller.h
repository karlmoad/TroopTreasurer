#ifndef DATAOBJECTCONTROLLER_H
#define DATAOBJECTCONTROLLER_H

#include <QList>
#include <QMap>
#include <QVariant>
#include <QJsonObject>
#include "resultstatus.h"
#include "dataaccessobject.h"
#include "relationship.h"
#include "dataresponsibility.h"

template <typename T>
class DataObjectController
{
public:
    explicit DataObjectController(DataResponsibility responsibility=DataResponsibility::SELF_ONLY);
    virtual ResultStatus load()=0;
    virtual void setArguments(const QMap<QString, QVariant>& args)=0;
    virtual void addRelationship(std::shared_ptr<Relationship> relationship, const QString& fieldName, const QString& fieldValue)=0;
    virtual std::shared_ptr<DataAccessObject<T>> dataAccessObject()=0;
    virtual int count() = 0;
    virtual int indexOf(const QString& key)=0;
    virtual const T& getObject(int index) = 0;
    virtual QJsonObject getJson(int index) =0;

protected:
    DataResponsibility _responsibility;
};

template<typename T>
DataObjectController<T>::DataObjectController(DataResponsibility responsibility): _responsibility(responsibility)
{}

template<typename T>
DataObjectController<T>::DataObjectController()
{}

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
