#ifndef TROOPTREASURER_DAO_H
#define TROOPTREASURER_DAO_H

#include <QModelIndex>
#include <QObject>
#include <QString>
#include <QMap>
#include "resultstatus.h"

template <typename T>
class DAOInterface
{
public:
    ResultStatus load(const QMap<QString,QString> &params)=0;
    ResultStatus add(const T& object)=0;
    ResultStatus update(const QModelIndex &index, const T& object)=0;
    ResultStatus delete(const QModelIndex &index, const T& object)=0;
};
#endif //TROOPTREASURER_DAO_H
