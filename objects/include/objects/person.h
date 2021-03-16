#ifndef PERSON_H
#define PERSON_H

#include <QString>
#include <QJsonObject>
#include <memory>

class Person
{
public:
    Person();
    explicit Person(const Person &copy);
    virtual ~Person();

    QString key() const;
    void setFirstName(const QString &name);
    QString firstName() const;
    void setLastName(const QString &name);
    QString lastName() const;
    void setMobile(const QString &phone);
    QString mobile() const;
    void setHome(const QString &phone);
    QString home() const;
    void setWork(const QString &phone);
    QString work() const;
    void setScout(bool isScout);
    bool isScout() const;

    Person& operator=(const Person &other);
    bool operator==(const Person &rhs) const;
    bool operator!=(const Person &rhs) const;
private:
    friend class PeopleDAO;
    explicit Person(const QJsonObject& json);

    class PersonImpl;
    std::shared_ptr<PersonImpl> impl;
};


#endif //PERSON_H
