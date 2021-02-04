#ifndef TROOPTREASURER_ACCOUNT_H
#define TROOPTREASURER_ACCOUNT_H

#include <QString>
#include <QJsonObject>
#include <memory>

class Account
{
public:
    explicit Account();
    explicit Account(const Account &copy);
    virtual ~Account();

    void setKey(QString const &key);
    QString key() const;
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
    void setOrg(QString const &org);
    QString org() const;
    const QJsonObject& json() const;

    //operators
    bool operator==(const Account &rhs) const;
    bool operator!=(const Account &rhs) const;
    Account& operator=(const Account &other);

private:
    explicit Account(const QJsonObject& json);
    friend class AccountsModel;
    void setParent(const QString& parent);
    QString parent() const;
    void setDisplayOrder(int order);
    int displayOrder() const;

private:
    class AccountImpl;
    std::shared_ptr<AccountImpl> impl;
};


#endif //TROOPTREASURER_ACCOUNT_H
