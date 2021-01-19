#ifndef TROOPTREASURER_DEPOSIT_H
#define TROOPTREASURER_DEPOSIT_H

#include <QObject>
#include <QString>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QJsonObject>
#include <QJsonValue>
#include <memory>
#include "transactions.h"
#include "fundsrecord.h"

namespace Transactions
{
    class Deposit
    {
    public:
        Deposit();
        explicit Deposit(const Deposit& copy);
        virtual ~Deposit();

        QString key() const;

        QDate date() const;
        void setDate(QDate date);

        double total() const;
        void setTotal(double value);

        bool finalized() const;
        void setFinalized(bool value);

        QDate finalizedDate() const;
        bool reconciled() const;

        void setReconciled(bool value);

        const QJsonObject &json() const;

        bool operator==(const Deposit &rhs) const;

        bool operator!=(const Deposit &rhs) const;

        Deposit& operator=(const Deposit &other);

    private:
        class DepositImpl;
        std::shared_ptr<DepositImpl> impl;


    private:
        friend class DepositsModel;
        explicit Deposit(const QSqlRecord& record);
        void setKey(const QString& key);

    };
}

#endif //TROOPTREASURER_DEPOSIT_H
