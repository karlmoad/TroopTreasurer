#ifndef TROOPTREASURER_DATATYPE_H
#define TROOPTREASURER_DATATYPE_H


#include <QString>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>

namespace DataTypes
{
    enum class Type{STRING = 0, INTEGER, DECIMAL,DATE, BIT, LITERAL,UNDEFINED};
    static const QList<QString> TypeStrings = {"string", "integer", "decimal", "date","bit","literal","undefined"};
    static const QList<bool> QuotedIndicators = {true, false, false,true, false, false, false};

    static QString TypeToString(DataTypes::Type type)
    {
        return TypeStrings[static_cast<int>(type)];
    };

    static Type StringToType(const QString& typeString)
    {
        if(TypeStrings.contains(typeString))
        {
            int idx = TypeStrings.indexOf(typeString, 0);
            if (idx >= 0 && idx < TypeStrings.size())
            {
                return static_cast<DataTypes::Type>(idx);
            }
        }
        return Type::UNDEFINED;
    };

    static bool IsQuotedType(DataTypes::Type type)
    {
        return QuotedIndicators[static_cast<int>(type)];
    };
}

class DataType
{
public:
    DataType();
    explicit DataType(DataTypes::Type type);
    explicit DataType(const QString& type);
    ~DataType();

    DataTypes::Type type() const;
    void setType(DataTypes::Type type);
    void setType(const QString& typeString);
    bool isQuoted() const;
    QString toString() const;

protected:
    DataTypes::Type _type;
};

#endif //TROOPTREASURER_DATATYPE_H
