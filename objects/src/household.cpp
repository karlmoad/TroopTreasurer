#include "objects/household.h"
#include "objects/dataobject.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

class Household::HouseholdImpl : public DataObject
{
public:
    HouseholdImpl(): DataObject(){}
    ~HouseholdImpl(){}
    explicit HouseholdImpl(const HouseholdImpl &other): DataObject(other){}
    explicit HouseholdImpl(const QJsonObject &json): DataObject(json){}
};

Household::Household() : impl(new HouseholdImpl())
{}

Household::Household(const Household &other)
{
    this->impl = std::shared_ptr<HouseholdImpl>(new HouseholdImpl(other.impl->json()));
}

Household::~Household()
{}

QString Household::key() const
{
    return impl->getValue("key").toString();
}

QString Household::name() const
{
    return impl->getValue("name").toString();
}

void Household::setName(const QString &name)
{
    impl->setValue("name", name);
}

QString Household::address1() const
{
    return impl->getValue("addr1").toString();
}

void Household::setAddress1(const QString &addr)
{
    impl->setValue("addr1",addr);
}

QString Household::address2() const
{
    return impl->getValue("addr2").toString();
}

void Household::setAddress2(const QString &addr)
{
    impl->setValue("addr2",addr);
}

QString Household::city() const
{
    return impl->getValue("city").toString();
}

void Household::setCity(const QString &city)
{
    impl->setValue("city",city);
}

QString Household::state() const
{
    return impl->getValue("state").toString();
}

void Household::setState(const QString &state)
{
    impl->setValue("state",state);
}

QString Household::zip() const
{
    return impl->getValue("zip").toString();
}

void Household::setZip(const QString &zip)
{
    impl->setValue("zip",zip);
}

Household &Household::operator=(const Household &other)
{
    if(*this != other)
    {
        this->impl = std::shared_ptr<HouseholdImpl>(new HouseholdImpl(other.impl->json()));
    }
    return *this;
}

bool Household::operator==(const Household &rhs) const
{
    return impl == rhs.impl;
}

bool Household::operator!=(const Household &rhs) const
{
    return !(rhs == *this);
}

Household::Household(const QJsonObject &json)
{
    this->impl = std::shared_ptr<HouseholdImpl>(new HouseholdImpl(json));
}
