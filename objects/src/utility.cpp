#include "objects/utility.h"
#include <QStandardPaths>

QString Utility::GenerateKey()
{
    return QUuid::createUuid().toString();
}

QString Utility::GetUserPreferencesDirectoryPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
}

QString Utility::GetUserPreferencesFilePath(const QString &filename)
{
    return QString("%1/%2").arg(GetUserPreferencesDirectoryPath(), filename);
}



