#include <objects/persondao.h>

PersonDAO::PersonDAO()
{

}

PersonDAO::~PersonDAO()
{

}

ResultStatus PersonDAO::load(const QMap<QString, QString> &params)
{
    return ResultStatus();
}

ResultStatus PersonDAO::add(const Person &object)
{
    return ResultStatus();
}

ResultStatus PersonDAO::update(const Person &object)
{
    return ResultStatus();
}

ResultStatus PersonDAO::remove(const Person &object)
{
    return ResultStatus();
}

Person PersonDAO::get(const QString &key)
{
    return Person();
}


