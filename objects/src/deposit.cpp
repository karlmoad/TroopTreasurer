#include "objects/deposit.h"
#include "objects/utility.h"
#include "objects/dates.h"

class Transactions::Deposit::DepositImpl
{
public:
    DepositImpl()
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

    explicit DepositImpl(const QJsonObject &json)
    {
        _base = json;
    }

    explicit DepositImpl(const QSqlRecord &record)
    {
        load(record);
    }

    const QJsonObject& json()
    {
        return _base;
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

private:
    QJsonObject _base;

    void load(const QSqlRecord &record)
    {
        _base["key"] = record.value(0).toString();
        _base["date"] = record.value(1).toString();
        _base["total"] = record.value(2).toDouble();
        _base["finalized"] = record.value(3).toInt() == 1;
        _base["finalized_date"] = record.value(4).toString();
        _base["reconciled"] = record.value(5).toInt() == 1;
    }
};

Transactions::Deposit::Deposit(): impl(new DepositImpl){}

Transactions::Deposit::Deposit(const Transactions::Deposit &copy)
{
    this->impl = std::shared_ptr<DepositImpl>(new DepositImpl(copy.json()));
}

Transactions::Deposit::Deposit(const QSqlRecord &record): impl(new DepositImpl(record)){}

Transactions::Deposit::~Deposit(){}

QString Transactions::Deposit::key() const
{
    return impl->getValue("key").toString();
}

QDate Transactions::Deposit::date() const
{
    QString v = impl->getValue("date").toString();
    return v.trimmed().length() > 0 ? QDate::fromString(v, DateFormats::DATABASE_FORMAT) : QDate(1900,1,1);
}

void Transactions::Deposit::setDate(QDate date)
{
    impl->setValue("date",date.toString(DateFormats::DATABASE_FORMAT));
}

double Transactions::Deposit::total() const
{
    return impl->getValue("total").toDouble(0);
}

void Transactions::Deposit::setTotal(double value)
{
    impl->setValue("total",value);
}

bool Transactions::Deposit::finalized() const
{
    return impl->getValue("finalized").toBool(false);
}

void Transactions::Deposit::setFinalized(bool value)
{
    impl->setValue("finalized",value);
    if(value)
    {
        impl->setValue("finalized_date",QDate::currentDate().toString(DateFormats::DATABASE_FORMAT));
    }
    else
    {
        impl->setValue("finalized_date",QDate(1900,1,1).toString(DateFormats::DATABASE_FORMAT));
    }
}

QDate Transactions::Deposit::finalizedDate() const
{
    QString v = impl->getValue("finalized_date").toString();
    return v.trimmed().length() > 0 ? QDate::fromString(v, DateFormats::DATABASE_FORMAT) : QDate(1900,1,1);
}

bool Transactions::Deposit::reconciled() const
{
    return impl->getValue("reconciled").toBool(false);
}

void Transactions::Deposit::setReconciled(bool value)
{
    impl->setValue("reconciled",value);
}

const QJsonObject &Transactions::Deposit::json() const
{
    return impl->json();
}

void Transactions::Deposit::setKey(const QString &key)
{
    impl->setValue("key",key);
}

bool Transactions::Deposit::operator==(const Transactions::Deposit &rhs) const
{
    return this->key().compare(rhs.key(), Qt::CaseSensitive) ==0;
}

bool Transactions::Deposit::operator!=(const Transactions::Deposit &rhs) const
{
    return !(rhs == *this);
}

Transactions::Deposit &Transactions::Deposit::operator=(const Transactions::Deposit &other)
{
    this->impl = std::shared_ptr<DepositImpl>(new DepositImpl(other.json()));
    return *this;
}
