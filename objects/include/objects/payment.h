#ifndef TROOPTREASURER_PAYMENT_H
#define TROOPTREASURER_PAYMENT_H

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

    class Payment
    {
    public:
        Payment();
        Payment(const Payment &copy);

        virtual ~Payment();

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

        bool finalized() const;
        void setFinalized(bool value);

        QDate finalizedDate() const;
        bool reconciled() const;

        void setReconciled(bool value);

        QString who() const;
        void setWho(const QString &value);

        QString what() const;
        void setWhat(const QString &value);

        const QJsonObject &json() const;

        bool operator==(const Payment &rhs) const;

        bool operator!=(const Payment &rhs) const;

        Payment& operator=(const Payment &other);


    private:
        friend class PaymentsModel;
        void setKey(const QString &key);
        explicit Payment(const QSqlRecord &record);
        class PaymentImpl;
        std::shared_ptr<PaymentImpl> impl;
    };
}

#endif //TROOPTREASURER_PAYMENT_H
