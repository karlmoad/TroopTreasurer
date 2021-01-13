#ifndef TROOPTREASURER_TRANSACTIOTYPES_H
#define TROOPTREASURER_TRANSACTIOTYPES_H

#include <QString>
#include <QList>
#include <QMap>

namespace Transactions
{
    namespace TransactionTypes
    {
        enum class Type
        {
            UNKNOWN = 0, CASH = 1, CHECK = 2, CREDIT_CARD = 3, PAYPAL = 4, IOU = 999,
        };
        static const QList<QString> TypeStrings = {"", "Cash", "Check", "Credit/Debit Card", "PayPal", "IOU"};

        static QString typeToString(TransactionTypes::Type type)
        {
            return TypeStrings[static_cast<int>(type)];
        }

        static TransactionTypes::Type stringToType(const QString &typeString)
        {
            if (TypeStrings.contains(typeString.trimmed()))
            {
                int idx = TypeStrings.indexOf(typeString.trimmed(), 0);
                if (idx >= 0 && idx < TypeStrings.count())
                {
                    return static_cast<TransactionTypes::Type>(idx);
                }
            }
            return TransactionTypes::Type::UNKNOWN;
        }

        static QMap<TransactionTypes::Type, QString> map()
        {
            QMap<TransactionTypes::Type, QString> m;
            m.insert(TransactionTypes::Type::UNKNOWN, TypeStrings[0]);
            m.insert(TransactionTypes::Type::CASH, TypeStrings[1]);
            m.insert(TransactionTypes::Type::CHECK, TypeStrings[2]);
            m.insert(TransactionTypes::Type::CREDIT_CARD, TypeStrings[3]);
            m.insert(TransactionTypes::Type::PAYPAL, TypeStrings[4]);
            m.insert(TransactionTypes::Type::IOU, TypeStrings[5]);
            return m;
        }

        static int typeToInt(TransactionTypes::Type type)
        {
            return static_cast<int>(type);
        }

        static TransactionTypes::Type intToType(int value)
        {
            return static_cast<TransactionTypes::Type>(value);
        }
    }
}




#endif //TROOPTREASURER_TRANSACTIOTYPES_H
