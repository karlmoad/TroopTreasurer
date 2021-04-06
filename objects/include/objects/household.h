#ifndef HOUSEHOLD_H
#define HOUSEHOLD_H

#include <QString>
#include <QJsonObject>
#include <memory>
#include "dataobject.h"
#include "dataobjectcontroller.h"

class Household: public DataObject
{
public:
    Household();
    explicit Household(const Household &other);
    virtual ~Household();

    virtual QString key() const override;
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
    virtual bool isNull() override;
    virtual QJsonObject json() const override;

    Household& operator=(const Household &other);
    bool operator==(const Household &rhs) const;
    bool operator!=(const Household &rhs) const;

private:
    friend class HouseholdController;
    explicit Household(const QJsonObject& json);
    class HouseholdImpl;
    std::shared_ptr<HouseholdImpl> impl;
};

class HouseholdController: public DataObjectController<Household>
{
public:
    REM_COPY_MOVE(HouseholdController)
    HouseholdController();
    ~HouseholdController();

    virtual ResultStatus load(const QMap<QString, QVariant> &args) override;
    virtual void setData(const QList<Household> &objects) override;
    virtual DataAccessObject<Household> dataAccessObject() override;
    virtual int count() override;
    virtual int indexOf(const QString &key) override;
    virtual Household getObject(int index) override;
    virtual QJsonObject getJson(int index) override;
};

#endif //HOUSEHOLD_H
