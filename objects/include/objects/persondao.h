#ifndef TROOPTREASURER_PERSONDAO_H
#define TROOPTREASURER_PERSONDAO_H

#include <memory>
#include "dao.h"
#include "person.h"

class PersonDAO: public DataAccessObject<Person>
{
public:
    REM_COPY_MOVE(PersonDAO)
    PersonDAO();
    virtual ~PersonDAO();

    virtual bool isHierarchical() override;

    virtual int count() override;

    virtual int indexOf(const QString &key) override;

    virtual ResultStatus loadObjects(const QMap<QString, QString> &params) override;

    virtual void loadObjects(const QList<Person> &items) override;

    virtual Person getObject(const QString &key) override;

    virtual Person getObject(int index) override;

    virtual QJsonObject getJson(const QString &key) override;

    virtual QJsonObject getJson(int index) override;

    virtual ResultStatus addObject(const Person &object) override;

    virtual ResultStatus updateObject(const Person &object) override;

    virtual ResultStatus removeObject(int index) override;

private:
    class PersonDAOImpl;
    std::shared_ptr<PersonDAOImpl> impl;
};


#endif //TROOPTREASURER_PERSONDAO_H
