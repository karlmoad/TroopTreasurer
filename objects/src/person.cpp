#include "objects/person.h"
#include "objects/dataobject.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

class Person::PersonImpl : public DataObject
{
public:
    PersonImpl(): DataObject(){}
    ~PersonImpl(){}
    PersonImpl(const PersonImpl& other): DataObject(other){}
    PersonImpl(const QJsonObject& json): DataObject(json){}
};

Person::Person(): impl(new PersonImpl())
{}

Person::Person(const Person &copy)
{
    this->impl = std::shared_ptr<PersonImpl>(new PersonImpl(copy.impl->json()));
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
        this->impl = std::shared_ptr<PersonImpl>(new PersonImpl(other.impl->json()));
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
