#ifndef TROOPTREASURER_PERSONDAO_H
#define TROOPTREASURER_PERSONDAO_H

#include <memory>
#include "dao.h"
#include "person.h"

class PersonDAO: public DAOInterface<Person>
{
public:
    REM_COPY_MOVE(PersonDAO)
    PersonDAO();
    virtual ~PersonDAO();

    virtual ResultStatus load(const QMap<QString, QString> &params) override;

    virtual Person get(const QString &key) override;

    virtual ResultStatus add(const Person &object) override;

    virtual ResultStatus update(const Person &object) override;

    virtual ResultStatus remove(const Person &object) override;
private:
    class PersonDAOImpl;
    std::shared_ptr<PersonDAOImpl> impl;
};


#endif //TROOPTREASURER_PERSONDAO_H
