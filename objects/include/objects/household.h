#ifndef TROOPTREASURER_HOUSEHOLD_H
#define TROOPTREASURER_HOUSEHOLD_H


#include <QString>
#include <QJsonObject>
#include <memory>

class Household
{
public:
    Household();
    explicit Household(const Household &other);
    virtual ~Household();

    QString key() const;
    QString name() const;
    void setName(const QString &name);
    QString address1() const;
    void setAddress1(const QString &addr);
    QString address2() const;
    void setAddress2(const QString &addr);
    QString city() const;
    void setCity(const QString &city);
    QString state() const;
    void setState(const QString &state);
    QString zip() const;
    void setZip(const QString &zip);

    Household& operator=(const Household &other);
    bool operator==(const Household &rhs) const;
    bool operator!=(const Household &rhs) const;

private:
    friend class HouseholdModel;
    explicit Household(const QJsonObject& json);

    class HouseholdImpl;
    std::shared_ptr<HouseholdImpl> impl;
};


#endif //TROOPTREASURER_HOUSEHOLD_H
