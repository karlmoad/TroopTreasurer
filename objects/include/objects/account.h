#ifndef TROOPTREASURER_ACCOUNT_H
#define TROOPTREASURER_ACCOUNT_H

#include <QString>
#include <QJsonObject>
#include <QWidget>
#include <memory>

class Account : std::enable_shared_from_this<Account>
{
public:
    explicit Account();
    explicit Account(const Account &copy);
    virtual ~Account();
    void setKey(QString const &key);
    QString key() const;
    void setParent(std::shared_ptr<Account> parent);
    std::shared_ptr<Account> parent() const;
    int subAccountCount();
    std::shared_ptr<Account> subAccount(int index);
    int subAccountIndex() const;
    void addSubAccount(Account sub);
    void removeSubAccount(int index);
    void setName(QString const &name);
    QString name() const;
    void setSourceKey(QString const &source);
    QString sourceKey() const;
    void setReported(bool value);
    bool isReported() const;
    void setRollup(bool value);
    bool isRollup() const;
    void setExternal(bool value);
    bool isExternal() const;
    void setClosed(bool value);
    bool isClosed() const;
    void setDisplayOrder(int order);
    int displayOrder() const;
    void setOrg(QString const &org);
    QString org() const;
    const QJsonObject& json() const;

    bool operator==(const Account &rhs) const;
    bool operator!=(const Account &rhs) const;
    Account& operator=(const Account &other);

private:
    class AccountImpl;
    std::shared_ptr<AccountImpl> impl;

    friend class AccountsModel;
    explicit Account(const QJsonObject& json);
};


#endif //TROOPTREASURER_ACCOUNT_H
