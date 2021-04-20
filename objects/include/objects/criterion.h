#ifndef CRITERION_H
#define CRITERION_H

#include <QVariant>
#include <QString>
#include <QList>
#include "operator.h"

class Criterion
{
public:
    Criterion();
    ~Criterion();

    QString getName() const;
    Operator getOperator() const;
    QList<QVariant> getValues() const;

    class Builder
    {
    public:
        Builder(const QString& name);
        ~Builder();
        Builder& setOperator(Operator op);
        Builder& addValue(const QVariant& value);
        Criterion build();
    private:
        Criterion *_building;
    };

private:
    QString _name;
    Operator _op;
    QList<QVariant> _values;
};


//builder
Criterion::Builder::Builder(const QString &name)
{
    _building = new Criterion();
    _building->_name = name;
}

Criterion::Builder::~Builder()
{
    delete _building;
}

Criterion::Builder& Criterion::Builder::setOperator(Operator op)
{
    _building->_op = op;
    return *this;
}

Criterion::Builder& Criterion::Builder::addValue(const QVariant &value)
{
    _building->_values.append(value);
    return *this;
}

Criterion Criterion::Builder::build()
{
    return *_building;
}

// class

Criterion::Criterion()
{}

Criterion::~Criterion()
{}

QString Criterion::getName() const
{
    return _name;
}

Operator Criterion::getOperator() const
{
    return _op;
}

QList<QVariant> Criterion::getValues() const
{
    return _values;
}

#endif //CRITERION_H
