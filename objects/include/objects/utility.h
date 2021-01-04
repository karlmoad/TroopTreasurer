#ifndef TROOPTREASURER_UTILITY_H
#define TROOPTREASURER_UTILITY_H

#include <QString>
#include <QUuid>

struct Utility
{
public:
    static QString GenerateKey();
    static QString GetUserPreferencesDirectoryPath();
    static QString GetUserPreferencesFilePath(const QString& filename);
};

#endif //TROOPTREASURER_UTILITY_H
