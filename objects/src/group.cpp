#include <objects/group.h>
#include "objects/dataobject.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

class Group::GroupImpl : public DataObject
{
public:
    GroupImpl(): DataObject(){}
    ~GroupImpl(){}

    explicit GroupImpl(const GroupImpl& other): DataObject(other){}
    explicit GroupImpl(const QJsonObject& json): DataObject(json){}
};

Group::Group() : impl(new GroupImpl())
{}

Group::Group(const Group &other)
{
    this->impl = std::shared_ptr<GroupImpl>(new GroupImpl(other.impl->json()));
}

Group::~Group()
{}

QString Group::key() const
{
    return impl->getValue("key").toString();
}

QString Group::description() const
{
    return impl->getValue("desc").toString();
}

void Group::setDescription(const QString &value)
{
    impl->setValue("desc",value);
}

Group &Group::operator=(const Group &other)
{
    if(*this != other)
    {
        impl = std::shared_ptr<GroupImpl>(new GroupImpl(other.impl->json()));
    }
    return *this;
}

Group::Group(const QJsonObject &json): impl(new GroupImpl(json))
{}

bool Group::operator==(const Group &rhs) const
{
    return impl == rhs.impl;
}

bool Group::operator!=(const Group &rhs) const
{
    return !(rhs == *this);
}
