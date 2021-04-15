#ifndef PERSON_H
#define PERSON_H

#include <QString>
#include <QJsonObject>
#include <memory>
#include "dataobject.h"
#include "dataobjectcontroller.h"
#include "dataaccessobject.h"
#include "household.h"

class Person: public DataObject
{
public:
    Person();
    explicit Person(const Person &copy);
    explicit Person(const QJsonObject& json);
    virtual ~Person();

    virtual QString key() const override;
    void setFirstName(const QString &name);
    QString firstName() const;
    void setLastName(const QString &name);
    QString lastName() const;
    void setMobile(const QString &phone);
    QString email() const;
    void setEmail(const QString &email);
    QString mobile() const;
    void setHome(const QString &phone);
    QString home() const;
    void setWork(const QString &phone);
    QString work() const;
    void setScout(bool isScout);
    bool isScout() const;
    void setMember(bool isMember);
    bool isMember() const;
    virtual bool isNull() override;
    virtual QJsonObject json() const override;

    std::shared_ptr<DataAccessObject<Household>> households();

    Person& operator=(const Person &other);
    bool operator==(const Person &rhs) const;
    bool operator!=(const Person &rhs) const;

private:
    class PersonImpl;
    std::shared_ptr<PersonImpl> impl;
};

class PersonController : public DataObjectController<Person>
{
public:
    PersonController(DataResponsibility responsibility);
    ~PersonController();

    virtual ResultStatus load() override;

    virtual void addRelationship(std::shared_ptr<Relationship>, const QString &fieldName, const QString &fieldValue) override;

    virtual void setArguments(const QMap<QString, QVariant> &args) override;
    virtual std::shared_ptr<DataAccessObject<Person>> dataAccessObject() override;
    virtual int count() override;
    virtual int indexOf(const QString &key) override;
    virtual const Person& getObject(int index) override;
    virtual QJsonObject getJson(int index) override;

private:
    class PersonControllerImpl;
    std::shared_ptr<PersonControllerImpl> impl;
};

#endif //PERSON_H
