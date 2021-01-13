#ifndef TROOPTREASURER_DATES_H
#define TROOPTREASURER_DATES_H

#include <QString>
#include <QDate>

namespace DateFormats
{
    const QString DATABASE_FORMAT = "yyyy-MM-dd";
    const QString UI_DISPLAY_FORMAT = "MM/dd/yyyy";
}

namespace DateLimits
{
    const QDate MinDate= QDate(2000,1,1);
    const QDate MaxDate= QDate(9999,12,31);
    const QDate NullDate = MaxDate;
}

#endif //TROOPTREASURER_DATES_H
