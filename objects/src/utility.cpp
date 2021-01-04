#include "objects/utility.h"

QString Utility::GenerateKey()
{
    return QUuid::createUuid().toString();
}

