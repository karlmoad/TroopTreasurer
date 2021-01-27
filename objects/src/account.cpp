#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QVector>
#include "objects/account.h"
#include "objects/utility.h"
#include "objects/dates.h"
#include "objects/dataobject.h"

class Account::AccountImpl : public DataObject
{
public:
    AccountImpl(): DataObject(){}
    AccountImpl(const AccountImpl &copy): DataObject(copy)
    {
        this->_parent = copy._parent;
        this->_subAccounts = copy._subAccounts;
    }

    AccountImpl(const QJsonObject& json): DataObject(json){}

    std::shared_ptr<Account> _parent;
    QVector<std::shared_ptr<Account>> _subAccounts;
};

Account::Account(): impl(new AccountImpl){}

Account::Account(const Account &copy): impl(new AccountImpl(copy.json())){}

Account::Account(const QJsonObject &json) : impl(new AccountImpl(json)){}

Account::~Account(){}

void Account::setKey(const QString &key)
{
    impl->setValue("key",key);
}

QString Account::key() const
{
    return impl->getValue("key").toString();
}

void Account::setParent(std::shared_ptr<Account> parent)
{
    impl->_parent = parent;
}

std::shared_ptr<Account> Account::parent() const
{
    return impl->_parent;
}

int Account::subAccountCount()
{
    return impl->_subAccounts.count();
}

std::shared_ptr<Account> Account::subAccount(int index)
{
    if(index >=0 && index < impl->_subAccounts.count())
    {
        return impl->_subAccounts.at(index);
    }
    return nullptr;
}

int Account::subAccountIndex() const
{
    if(impl->_parent)
    {
        return impl->_parent->impl->_subAccounts.indexOf(std::const_pointer_cast<Account>(shared_from_this()));
    }
    return 0;
}

void Account::addSubAccount(Account sub)
{
    std::shared_ptr<Account> acct = std::shared_ptr<Account>(new Account(sub));
    acct->setParent(shared_from_this());
    impl->_subAccounts.append(acct);
}

void Account::removeSubAccount(int index)
{
    if(index < impl->_subAccounts.count())
    {
        auto acct = impl->_subAccounts.takeAt(index);
        if(acct->parent().get() == this)
        {
            acct->setParent(nullptr);
        }
    }
}

void Account::setName(const QString &name)
{
    impl->setValue("name",name);
}

QString Account::name() const
{
    return impl->getValue("name").toString();
}

void Account::setSourceKey(const QString &source)
{
    impl->setValue("source_key", source);
}

QString Account::sourceKey() const
{
    return impl->getValue("source_key").toString();
}

void Account::setReported(bool value)
{
    impl->setValue("reported",value);
}

bool Account::isReported() const
{
    return impl->getValue("reported").toBool(false);
}

void Account::setRollup(bool value)
{
    impl->setValue("rollup",value);
}

bool Account::isRollup() const
{
    return impl->getValue("rollup").toBool(false);
}

void Account::setExternal(bool value)
{
    impl->setValue("external",value);
}

bool Account::isExternal() const
{
    return impl->getValue("external").toBool(false);
}

void Account::setClosed(bool value)
{
    impl->setValue("closed",value);
}

bool Account::isClosed() const
{
    return impl->getValue("closed").toBool(false);
}

void Account::setDisplayOrder(int order)
{
    impl->setValue("display_order",order);
}

int Account::displayOrder() const
{
    return impl->getValue("display_order").toInt(0);
}

void Account::setOrg(const QString &org)
{
    impl->setValue("org",org);
}

QString Account::org() const
{
    return impl->getValue("org").toString();
}

const QJsonObject &Account::json() const
{
    return impl->json();
}

Account &Account::operator=(const Account &other)
{
    impl=std::shared_ptr<AccountImpl>(new AccountImpl(*(other.impl.get())));
    return *this;
}

bool Account::operator==(const Account &rhs) const
{
    return this->key().compare(rhs.key(),Qt::CaseSensitive) == 0;
}

bool Account::operator!=(const Account &rhs) const
{
    return !(rhs == *this);
}


