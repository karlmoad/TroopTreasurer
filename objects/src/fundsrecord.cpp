#include "objects/fundsrecord.h"
#include "objects/utility.h"
#include "objects/dates.h"

class Transactions::FundsRecord::FundsRecordImpl
{
public:
    FundsRecordImpl()
    {
        if(!_base.contains("key"))
        {
            _base["key"] = Utility::GenerateKey();
        }
        if(!_base.contains("date"))
        {
            _base["date"] = QDate::currentDate().toString(DateFormats::DATABASE_FORMAT);
        }
    }

    explicit FundsRecordImpl(const QJsonObject& json)
    {
        _base = json;
    }

    explicit FundsRecordImpl(const QSqlRecord &record)
    {
        load(record);
    }

    QJsonValue getValue(const QString& key)
    {
        if(_base.contains(key))
        {
            return _base[key];
        }
        return QJsonValue();
    }

    void setValue(const QString& key, const QJsonValue& value)
    {
        _base[key]=value;
    }

    const QJsonObject& json()
    {
        return _base;
    }

private:
    QJsonObject _base;

    void load(const QSqlRecord &record)
    {
        _base["key"] = record.value(0).toString();
        _base["date"] = record.value(1).toString();
        _base["amount"] = record.value(2).toDouble();
        _base["method"] = record.value(3).toInt();
        _base["ref"]= record.value(4).toString();
        _base["comments"] = record.value(5).toString();
        _base["who"] = record.value(6).toString();
        _base["what"] = record.value(7).toString();
        _base["deposit"] = record.value(8).toString();
        _base["reconciled"] = record.value(9).toInt() == 1;
    }
};

Transactions::FundsRecord::FundsRecord(): impl(new FundsRecordImpl){}

Transactions::FundsRecord::FundsRecord(const FundsRecord &copy)
{
    this->impl = std::shared_ptr<FundsRecordImpl>(new FundsRecordImpl(copy.json()));
}

Transactions::FundsRecord::FundsRecord(const QSqlRecord &record): impl(new FundsRecordImpl(record)){}

Transactions::FundsRecord::~FundsRecord()
{

}

QString Transactions::FundsRecord::key() const
{
    return QString();
}

QDate Transactions::FundsRecord::date() const
{
    return QDate();
}

void Transactions::FundsRecord::setDate(QDate date)
{

}

double Transactions::FundsRecord::amount() const
{
    return 0;
}

void Transactions::FundsRecord::setAmount(double value)
{

}

Transactions::TransactionTypes::Type Transactions::FundsRecord::method() const
{
    return Transactions::TransactionTypes::Type::PAYPAL;
}

void Transactions::FundsRecord::setMethod(Transactions::TransactionTypes::Type type)
{

}

QString Transactions::FundsRecord::referenceValue() const
{
    return QString();
}

void Transactions::FundsRecord::setReferenceValue(const QString &value)
{

}

QString Transactions::FundsRecord::comments() const
{
    return QString();
}

void Transactions::FundsRecord::setComments(const QString &value)
{

}

QString Transactions::FundsRecord::who() const
{
    return QString();
}

void Transactions::FundsRecord::setWho(const QString &value)
{

}

QString Transactions::FundsRecord::what() const
{
    return QString();
}

void Transactions::FundsRecord::setWhat(const QString &value)
{

}

const QJsonObject &Transactions::FundsRecord::json() const
{
    return <#initializer#>;
}

void Transactions::FundsRecord::setKey(const QString &key)
{

}
