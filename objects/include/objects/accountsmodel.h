#ifndef TROOPTREASURER_ACCOUNTSMODEL_H
#define TROOPTREASURER_ACCOUNTSMODEL_H


#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QVariant>
#include <memory>
#include "account.h"

class AccountsModel: public QAbstractItemModel
{
Q_OBJECT
public:
    explicit AccountsModel(QObject *parent=nullptr);
    virtual ~AccountsModel();

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    std::shared_ptr<Account> getAccount(const QModelIndex& index) const;
    void addAccount(const Account& account, const QModelIndex &parent);
    void updateAccount(const QModelIndex &index);
    void moveAccount(const QModelIndex &index, const QModelIndex& parent);
    void deleteAccount(const QModelIndex& index);
    void load();
    static bool isRootAccount(std::shared_ptr<Account> account);

private:
    class AccountsModelImpl;
    std::shared_ptr<AccountsModelImpl> impl;
};

class AccountsProxyModel: public QSortFilterProxyModel
{
Q_OBJECT
public:
    explicit AccountsProxyModel(QObject *parent= nullptr);
    virtual ~AccountsProxyModel();
    void setModelIndexFilter(const QModelIndex& index);
    void setAccountClosedViewFilter(bool showClosed=true);
protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
private:
    QModelIndex contextFilter;
    bool showClosedAccounts;
};


#endif //TROOPTREASURER_ACCOUNTSMODEL_H
