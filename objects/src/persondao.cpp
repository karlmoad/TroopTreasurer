#include <objects/persondao.h>
#include <QString>
#include <QList>

namespace PersonSQL
{
    static const QString KeyField = QString("ID");
    static const QList<QString> NonKeyFields({"FIRST_NAME, LAST_NAME, EMAIL, MOBILE, HOME, WORK, SCOUT_FLAG, MEMBER_FLAG"});
    static const QString Table = QString("PEOPLE");
    static const QString stmtRetrieveByHousehold = QString("SELECT %1 FROM PEOPLE P"
                                                           " JOIN PEOPLE_HOUSEHOLD_LINK PHL ON P.ID = PHL.PERSON"
                                                           " JOIN HOUSEHOLD H ON PHL.HOUSEHOLD = H.ID "
                                                           " WHERE H.ID = %2");
    static const QString stmtRetrieveByGroup = "";
    static const QString stmtRetrieveByAccount = "";
}

class PersonDAO::PersonDAOImpl
{


};

PersonDAO::PersonDAO()
{

}

PersonDAO::~PersonDAO()
{

}

bool PersonDAO::isHierarchical()
{
    return false;
}

int PersonDAO::count()
{
    return 0;
}

int PersonDAO::indexOf(const QString &key)
{
    return 0;
}

ResultStatus PersonDAO::loadObjects(const QMap<QString, QString> &params)
{
    return ResultStatus();
}

Person PersonDAO::getObject(const QString &key)
{
    return Person();
}

Person PersonDAO::getObject(int index)
{
    return Person();
}

QJsonObject PersonDAO::getJson(const QString &key)
{
    return QJsonObject();
}

QJsonObject PersonDAO::getJson(int index)
{
    return QJsonObject();
}

ResultStatus PersonDAO::addObject(const Person &object)
{
    return ResultStatus();
}

ResultStatus PersonDAO::updateObject(const Person &object)
{
    return ResultStatus();
}

ResultStatus PersonDAO::removeObject(int index)
{
    return ResultStatus();
}

void PersonDAO::loadObjects(const QList<Person> &items)
{

}




