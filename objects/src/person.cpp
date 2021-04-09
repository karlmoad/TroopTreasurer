#include "objects/person.h"
#include "objects/dataobject.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "objects/databaseschema.h"
#include "objects/tableschema.h"
#include "objects/relationship.h"
#include "objects/databasecontroller.h"

class Person::PersonImpl : public DataObjectImpl
{
public:
    PersonImpl(): DataObjectImpl(){}
    ~PersonImpl(){}
    PersonImpl(const PersonImpl& other): DataObjectImpl(other){}
    PersonImpl(const QJsonObject& json): DataObjectImpl(json){}
};

Person::Person(): impl(new PersonImpl)
{}

Person::Person(const Person &copy)
{
    this->impl = std::shared_ptr<PersonImpl>(new PersonImpl(*(copy.impl.get())));
}

Person::~Person()
{}

QString Person::key() const
{
    return impl->getValue("key").toString();
}

void Person::setFirstName(const QString &name)
{
    impl->setValue("firstname", name);
}

QString Person::firstName() const
{
    return impl->getValue("firstname").toString();
}

void Person::setLastName(const QString &name)
{
    impl->setValue("lastname",name);
}

QString Person::lastName() const
{
    return impl->getValue("lastname").toString();
}

void Person::setMobile(const QString &phone)
{
    impl->setValue("mobilephone", phone);
}

QString Person::mobile() const
{
    return impl->getValue("mobilephone").toString();
}

void Person::setHome(const QString &phone)
{
    impl->setValue("homephone", phone);
}

QString Person::home() const
{
    return impl->getValue("homephone").toString();
}

void Person::setWork(const QString &phone)
{
    impl->setValue("workphone",phone);
}

QString Person::work() const
{
    return impl->getValue("workphone").toString();
}

void Person::setScout(bool isScout)
{
    impl->setValue("scout",isScout);
}

bool Person::isScout() const
{
    return impl->getValue("scout").toBool(false);
}

Person &Person::operator=(const Person &other)
{
    if(*this != other)
    {
        impl.reset();
        impl = std::shared_ptr<PersonImpl>(new PersonImpl(*(other.impl.get())));
    }
    return *this;
}

Person::Person(const QJsonObject &json)
{
    this->impl = std::shared_ptr<PersonImpl>(new PersonImpl(json));
}

bool Person::operator==(const Person &rhs) const
{
    return impl == rhs.impl;
}

bool Person::operator!=(const Person &rhs) const
{
    return !(rhs == *this);
}

bool Person::isNull()
{
    return impl==nullptr && impl->isNull();
}

QJsonObject Person::json() const
{
    return impl != nullptr ? impl->json() : QJsonObject();
}

std::shared_ptr<DataAccessObject<Household>> Person::households()
{
    return std::shared_ptr<DataAccessObject<Household>>();
}

// CONTROLLER

class PersonController::PersonControllerImpl : public DataObjectController<Person>,
                            public DataObjectControllerAddBinding<Person>,
                            public DataObjectControllerUpdateBinding<Person>,
                            public DataObjectControllerRemoveBinding<Person>
{
public:
    PersonControllerImpl() : _dao(new DataAccessObject<Person>)
    {
        _dao->bindContextItemCount(std::bind(&PersonController::PersonControllerImpl::count, this));
        _dao->bindContextIndexForKey(std::bind(&PersonController::PersonControllerImpl::indexOf, this, std::placeholders::_1));
        _dao->bindContextGet(std::bind(&PersonController::PersonControllerImpl::getObject, this, std::placeholders::_1));
        _dao->bindContextJson(std::bind(&PersonController::PersonControllerImpl::getJson, this, std::placeholders::_1));
        _dao->bindContextAdd(std::bind(&PersonController::PersonControllerImpl::add, this, std::placeholders::_1));
        _dao->bindContextUpdate(std::bind(&PersonController::PersonControllerImpl::update, this, std::placeholders::_1));
        _dao->bindContextRemove(std::bind(&PersonController::PersonControllerImpl::remove, this, std::placeholders::_1));
        _databaseController = std::shared_ptr<DatabaseController>(new DatabaseController(DatabaseSchema::retrieve()->getTable("HOUSEHOLD")));
    }
    ~PersonControllerImpl(){}

    virtual void setArguments(const QMap<QString, QVariant> &args) override
    {
        _databaseController->setArguments(args);
    }

    virtual ResultStatus load() override
    {
        QList<QJsonObject> records;
        auto rs = _databaseController->load(records);
        if(rs.status() == ResultStatus::Status::SUCCESS)
        {
            _objects.clear();
            for(int i = 0; i < records.count(); i++)
            {
                _objects.insert(i, Person(records[i]));
                _keyIndex.insert(_objects[i].key(), i);
            }
        }
        return rs;
    }

    virtual void addRelationship(std::shared_ptr<Relationship> relationship, const QString &fieldName, const QString &fieldValue) override
    {
        _databaseController->addRelationship(relationship, fieldName, fieldValue);
    }

    virtual std::shared_ptr<DataAccessObject<Person>> dataAccessObject() override
    {
        return _dao;
    }

    virtual int count() override
    {
        return _objects.count();
    }

    int indexOf(const QString &key) override
    {
        if(_keyIndex.contains(key))
        {
            return _keyIndex[key];
        }
        return -1;
    }

    const Person& getObject(int index) override
    {
        if(index < _objects.count())
        {
            return _objects[index];
        }
        return Person();
    }

    QJsonObject getJson(int index) override
    {
        return getObject(index).json();
    }

    ResultStatus add(const Person &t) override
    {
        return ResultStatus();
    }

    ResultStatus update(const Person &t) override
    {
        return ResultStatus();
    }

    ResultStatus remove(int i) override
    {
        return ResultStatus();
    }

private:
    std::shared_ptr<DataAccessObject<Person>> _dao;
    std::shared_ptr<DatabaseController> _databaseController;
    QList<Person> _objects;
    QMap<QString, int> _keyIndex;
};

PersonController::PersonController() : impl(new PersonControllerImpl)
{}

PersonController::~PersonController()
{}

void PersonController::setArguments(const QMap<QString, QVariant> &args)
{
    return impl->setArguments(args);
}

std::shared_ptr<DataAccessObject<Person>> PersonController::dataAccessObject()
{
    return impl->dataAccessObject();
}

int PersonController::count()
{
    return impl->count();
}

int PersonController::indexOf(const QString &key)
{
    return impl->indexOf(key);
}

const Person& PersonController::getObject(int index)
{
    return impl->getObject(index);
}

QJsonObject PersonController::getJson(int index)
{
    return impl->getJson(index);
}

ResultStatus PersonController::load()
{
    return impl->load();
}

void PersonController::addRelationship(std::shared_ptr<Relationship> relationship, const QString &fieldName, const QString &fieldValue)
{
    impl->addRelationship(relationship, fieldName, fieldValue);
}


