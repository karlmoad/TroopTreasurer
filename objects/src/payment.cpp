#include "objects/payment.h"
#include "objects/utility.h"
#include "objects/dates.h"

class Transactions::Payment::PaymentImpl
{
public:
    PaymentImpl()
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

    explicit PaymentImpl(const QJsonObject& json)
    {
        _base = json;
    }

    explicit PaymentImpl(const QSqlRecord &record)
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


    ~PaymentImpl()=default;

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
        _base["finalized"] = record.value(6).toInt() == 1;
        _base["finalized_date"] = record.value(7).toString();
        _base["reconciled"] = record.value(8).toInt() == 1;
        _base["who"] = record.value(9).toString();
        _base["what"] = record.value(10).toString();
    }
};

Transactions::Payment::Payment() : impl(new PaymentImpl){}

Transactions::Payment::Payment(const QSqlRecord &record): impl(new PaymentImpl(record)){}

Transactions::Payment::~Payment(){}

void Transactions::Payment::setKey(const QString &key)
{
    impl->setValue("key",key);
}

QString Transactions::Payment::key() const
{
    return impl->getValue("key").toString();
}

QDate Transactions::Payment::date() const
{
    QString v = impl->getValue("date").toString();
    return v.trimmed().length() > 0 ? QDate::fromString(v, DateFormats::DATABASE_FORMAT) : QDate(1900,1,1);
}

void Transactions::Payment::setDate(QDate date)
{
    impl->setValue("date",date.toString(DateFormats::DATABASE_FORMAT));
}

double Transactions::Payment::amount() const
{
    return impl->getValue("amount").toDouble(0);
}

void Transactions::Payment::setAmount(double value)
{
    impl->setValue("amount", value);
}

Transactions::TransactionTypes::Type Transactions::Payment::method() const
{
    return static_cast<TransactionTypes::Type>(impl->getValue("method").toInt(0));
}

void Transactions::Payment::setMethod(Transactions::TransactionTypes::Type type)
{
    impl->setValue("method",static_cast<int>(type));
}

QString Transactions::Payment::referenceValue() const
{
    return impl->getValue("ref").toString();
}

void Transactions::Payment::setReferenceValue(const QString &value)
{
impl->setValue("ref", value);
}

bool Transactions::Payment::finalized() const
{
    return impl->getValue("finalized").toBool(false);
}

void Transactions::Payment::setFinalized(bool value)
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

QDate Transactions::Payment::finalizedDate() const
{
    QString v = impl->getValue("finalized_date").toString();
    return v.trimmed().length() > 0 ? QDate::fromString(v, DateFormats::DATABASE_FORMAT) : QDate(1900,1,1);
}

bool Transactions::Payment::reconciled() const
{
    return impl->getValue("reconciled").toBool(false);
}

void Transactions::Payment::setReconciled(bool value)
{
    impl->setValue("reconciled",value);
}

QString Transactions::Payment::who() const
{
    return impl->getValue("who").toString();
}

void Transactions::Payment::setWho(const QString &value)
{
    impl->setValue("who",value);
}

QString Transactions::Payment::what() const
{
    return impl->getValue("what").toString();
}

void Transactions::Payment::setWhat(const QString &value)
{
    impl->setValue("what",value);
}

const QJsonObject &Transactions::Payment::json() const
{
    return impl->json();
}

QString Transactions::Payment::comments() const
{
    return impl->getValue("comments").toString();
}

void Transactions::Payment::setComments(const QString &value)
{
    impl->setValue("comments",value);
}

bool Transactions::Payment::operator==(const Transactions::Payment &rhs) const
{
    return this->key().compare(rhs.key(), Qt::CaseSensitive) ==0;
}

bool Transactions::Payment::operator!=(const Transactions::Payment &rhs) const
{
    return !(rhs == *this);
}

Transactions::Payment::Payment(const Transactions::Payment &copy)
{
    this->impl = std::shared_ptr<PaymentImpl>(new PaymentImpl(copy.json()));
}

Transactions::Payment &Transactions::Payment::operator=(const Transactions::Payment &other)
{
    this->impl = std::shared_ptr<PaymentImpl>(new PaymentImpl(other.json()));
    return *this;
}


