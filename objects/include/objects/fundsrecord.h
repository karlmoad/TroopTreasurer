#ifndef TROOPTREASURER_FUNDSRECORD_H
#define TROOPTREASURER_FUNDSRECORD_H

#include <QObject>
#include <QString>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QJsonObject>
#include <QJsonValue>
#include <memory>
#include "transactions.h"

namespace Transactions
{
    class FundsRecord
    {
        FundsRecord();
        explicit FundsRecord(const FundsRecord &copy);
        virtual ~FundsRecord();

        QString key() const;

        QDate date() const;
        void setDate(QDate date);

        double amount() const;
        void setAmount(double value);

        TransactionTypes::Type method() const;
        void setMethod(TransactionTypes::Type type);

        QString referenceValue() const;
        void setReferenceValue(const QString &value);

        QString comments() const;
        void setComments(const QString& value);

        QString who() const;
        void setWho(const QString &value);

        QString what() const;
        void setWhat(const QString &value);

        const QJsonObject &json() const;

    private:
        class FundsRecordImpl;
        std::shared_ptr<FundsRecordImpl> impl;

    private:
        explicit FundsRecord(const QSqlRecord &record);
        void setKey(const QString &key);

    };
}


#endif //TROOPTREASURER_FUNDSRECORD_H
