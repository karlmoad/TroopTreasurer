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
    return impl->getValue("key").toString();
}

QDate Transactions::FundsRecord::date() const
{
    QString v = impl->getValue("date").toString();
    return v.trimmed().length() > 0 ? QDate::fromString(v, DateFormats::DATABASE_FORMAT) : QDate(1900,1,1);
}

void Transactions::FundsRecord::setDate(QDate date)
{
    impl->setValue("date",date.toString(DateFormats::DATABASE_FORMAT));
}

double Transactions::FundsRecord::amount() const
{
    return impl->getValue("amount").toDouble(0);
}

void Transactions::FundsRecord::setAmount(double value)
{
    impl->setValue("amount", value);
}

Transactions::TransactionTypes::Type Transactions::FundsRecord::method() const
{
    return static_cast<TransactionTypes::Type>(impl->getValue("method").toInt(0));
}

void Transactions::FundsRecord::setMethod(Transactions::TransactionTypes::Type type)
{
    impl->setValue("method",static_cast<int>(type));
}

QString Transactions::FundsRecord::referenceValue() const
{
    return impl->getValue("ref").toString();
}

void Transactions::FundsRecord::setReferenceValue(const QString &value)
{
    impl->setValue("ref", value);
}

QString Transactions::FundsRecord::comments() const
{
    return impl->getValue("comments").toString();
}

void Transactions::FundsRecord::setComments(const QString &value)
{
    impl->setValue("comments",value);
}

QString Transactions::FundsRecord::who() const
{
    return impl->getValue("who").toString();
}

void Transactions::FundsRecord::setWho(const QString &value)
{
    impl->setValue("who",value);
}

QString Transactions::FundsRecord::what() const
{
    return impl->getValue("what").toString();
}

void Transactions::FundsRecord::setWhat(const QString &value)
{
    impl->setValue("what",value);
}

const QJsonObject &Transactions::FundsRecord::json() const
{
    return impl->json();
}

void Transactions::FundsRecord::setKey(const QString &key)
{
    impl->setValue("key",key);
}

QString Transactions::FundsRecord::depositKey() const
{
    return impl->getValue("deposit").toString();
}

bool Transactions::FundsRecord::reconciled() const
{
    return impl->getValue("reconciled").toBool(false);
}

void Transactions::FundsRecord::setReconciled(bool value)
{
    impl->setValue("reconciled",value);
}

void Transactions::FundsRecord::setDepositKey(const QString &deposit)
{
    impl->setValue("deposit", deposit);
}
