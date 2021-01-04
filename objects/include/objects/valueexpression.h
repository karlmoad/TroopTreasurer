#ifndef TROOPTREASURER_VALUEEXPRESSION_H
#define TROOPTREASURER_VALUEEXPRESSION_H


#include <QVariant>
#include <QDebug>
#include <QString>
#include "expression.h"

class ValueOf: public Expression
{
public:
    ValueOf(QVariant  value):Expression(0), _value(std::move(value)){};
    virtual ~ValueOf(){};

    virtual QVariant execute(const QList<QString> &record) override
    {
        return _value;
    }

private:
    QVariant _value;
};

class RecordValue: public Expression
{
public:
    RecordValue(int fieldRef=-1):Expression(0), _v(fieldRef){};
    virtual ~RecordValue(){};

    virtual QVariant execute(const QList<QString> &record) override
    {
        if(_v != -1)
        {
            if(_v < record.size())
            {
                return record.at(_v);
            }
        }
        return QVariant();
    }

private:
    int _v;
};

class Trim: public Expression
{
public:
    Trim():Expression(1){};
    virtual ~Trim(){};

    virtual QVariant execute(const QList<QString> &record) override
    {
        if(_numArgz == 1 && _argz.size() >= _numArgz)
        {
            QVariant v = _argz[0]->getValue(record);
            if(!v.isNull())
            {
                return v.toString().trimmed();
            }
        }
        return QVariant();
    };
};

class RemSpecial: public Expression
{
public:
    RemSpecial():Expression(1){};
    virtual ~RemSpecial(){};

    virtual QVariant execute(const QList<QString> &record) override
    {
        if(_argz.size()==1)
        {
            QVariant v = _argz[0]->getValue(record);
            if(!v.isNull())
            {
                return v.toString().remove(QChar('\\'),Qt::CaseInsensitive).remove(QChar('\''), Qt::CaseInsensitive);
            }
        }
        return QVariant();
    };
};

class Concat: public Expression
{
public:
    Concat(int num): Expression(num){}
    virtual ~Concat(){}

    virtual QVariant execute(const QList<QString> &record) override
    {
        QString out;
        for(Argument *arg: _argz)
        {
            QVariant v = arg->getValue(record);
            if(!v.isNull())
            {
                out.append(v.toString());
            }
        }
        return out;
    }
};

#endif //TROOPTREASURER_VALUEEXPRESSION_H
