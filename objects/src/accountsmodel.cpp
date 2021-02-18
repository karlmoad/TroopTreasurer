#include "objects/accountsmodel.h"
#include <QUuid>
#include <QMap>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlResult>
#include "objects/hierarchyitem.h"
#include "objects/objecterror.h"
#include <QDebug>


namespace Accounts
{
    static const QString RootAccountKey = "{00000000-0000-0000-0000-000000000000}";
}

namespace AccountsSql
{
    static const QString Fields = QString("ACCT_KEY, ACCT_NAME, ACCT_PARENT, SOURCE_KEY, REPORTED_FLAG, ROLLUP_FLAG, DISPLAY_ORDER, EXTERNAL_FLAG, CLOSED_FLAG, ORG");
    static const QString SelectStmt = QString("SELECT " + Fields + " FROM ACCOUNT_MASTER WHERE ACCT_PARENT IS NOT NULL ORDER BY ACCT_PARENT, DISPLAY_ORDER");
    static const QString UpdateStmt = QString("UPDATE ACCOUNT_MASTER SET %1 WHERE ACCT_KEY='%2'");
    static const QString InsertStmt = QString("INSERT INTO ACCOUNT_MASTER (" + Fields + ") VALUES (%1)");
    static const QString DeleteStmt = QString("UPDATE ACCOUNT_MASTER SET ACCT_PARENT = NULL WHERE ACCT_KEY='%1'");
    static const QString ExistsStmt = QString("SELECT COUNT(*) FROM ACCOUNT_MASTER WHERE ACCT_KEY='%1'");
    static const QString DeletableCheckStmt = QString("SELECT ACCT_KEY, CLOSED_FLAG, (SELECT COUNT(ACCT_KEY) FROM ACCOUNT_MASTER WHERE ACCT_PARENT ='%1') AS SUB_COUNT FROM ACCOUNT_MASTER WHERE ACCT_KEY='%1'");
    static const QString UnassociatedSourceAccountsStmt = QString("SELECT DISTINCT JRNL.ACCT_HASH, JRNL.ACCT_NAME"
                                                                  " FROM TROOP_TRANSACTION_JOURNAL JRNL"
                                                                  " WHERE NOT EXISTS (SELECT AM.SOURCE_KEY FROM ACCOUNT_MASTER AM WHERE AM.SOURCE_KEY = JRNL.ACCT_HASH)");
}

class AccountsModel::AccountsModelImpl
{
public:
    void load()
    {
        QMap<QString, QList<QString>> parent2ChildRef;
        auto rootAcct = std::shared_ptr<Account>(new Account);
        rootAcct->setKey(Accounts::RootAccountKey);
        rootAcct->setName("Root Account");
        rootAcct->setReported(true);
        rootAcct->setRollup(true);
        rootAcct->setSourceKey("--root--");
        _accounts.insert(rootAcct->key(), rootAcct);

        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        if(!db.open())
        {
            ObjectError err(QString("Insert record failed: Database failed to open, " + QString(db.lastError().databaseText())), static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
            err.raise();
            return;
        }
        QSqlQuery q(db);

        QString sql = AccountsSql::SelectStmt;
        if(!q.exec(sql))
        {
            ObjectError err(q.lastError().databaseText(), static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
            err.raise();
            return;
        }

        while (q.next())
        {
            QSqlRecord rec = q.record();
            QJsonObject meta;
            meta["key"] = rec.value("ACCT_KEY").toString();
            meta["name"] = rec.value("ACCT_NAME").toString();
            meta["parent"] = rec.value("ACCT_PARENT").toString();
            meta["source_key"] = rec.value("SOURCE_KEY").toString();
            meta["reported"] = rec.value("REPORTED_FLAG").toInt() == 1;
            meta["rollup"] = rec.value("ROLLUP_FLAG").toInt() == 1;
            meta["external"] = rec.value("EXTERNAL_FLAG").toInt() == 1;
            meta["closed"] = rec.value("CLOSED_FLAG").toInt() == 1;
            meta["display_order"] = rec.value("DISPLAY_ORDER").toInt();
            meta["org"] = rec.value("ORG").toString();

            auto acct = std::shared_ptr<Account>(new Account(meta));
            _accounts.insert(acct->key(), acct);

            if (!parent2ChildRef.contains(acct->parent()))
            {
                parent2ChildRef.insert(acct->parent(), QList<QString>());
            }
            parent2ChildRef[acct->parent()].append(acct->key());

        }

        _root = process(parent2ChildRef, Accounts::RootAccountKey);
    }

    static bool insertAccount(const Account& account, QString& message)
    {
        QString sql = AccountsSql::InsertStmt;
        QString buffer = QString("'%1'").arg(account.key());
        buffer.append(QString(",'%1'").arg(account.name()));
        buffer.append(QString(",'%1'").arg(account.parent().trimmed().length() == 0 ? Accounts::RootAccountKey : account.parent()));
        buffer.append(QString(",'%1'").arg(account.sourceKey()));
        buffer.append(QString(",%1").arg(account.isReported() ? "1" : "0"));
        buffer.append(QString(",%1").arg(account.isRollup() ? "1" : "0"));
        buffer.append(QString(",%1").arg(QString::number(account.displayOrder())));
        buffer.append(QString(",%1").arg(account.isExternal() ? "1" : "0"));
        buffer.append(QString(",%1").arg(account.isClosed() ? "1" : "0"));
        buffer.append(QString(",'%1'").arg(account.org()));

        QString stmt = sql.arg(buffer);

        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        if(!db.open())
        {
            message = QString("Insert record failed: Database failed to open, " + QString(db.lastError().databaseText()));
            return false;
        }

        QSqlQuery insert(db);
        if(!insert.exec(stmt))
        {
            message = QString("Insert record failed: " + insert.lastError().text() + "\n\nSQL:" + sql);
            return false;
        }
        else
        {
            return true;
        }
    }

    static bool updateAccount(const Account& account, QString& message)
    {
        QString buffer = QString("ACCT_NAME = '%1'").arg(account.name().trimmed());
        buffer.append(QString(",ACCT_PARENT='%1'").arg(account.parent()));
        buffer.append(QString(",SOURCE_KEY='%1'").arg(account.sourceKey()));
        buffer.append(QString(",REPORTED_FLAG=%1").arg(account.isReported() ? "1" : "0"));
        buffer.append(QString(",ROLLUP_FLAG=%1").arg(account.isRollup() ? "1" : "0"));
        buffer.append(QString(",DISPLAY_ORDER=%1").arg(QString::number(account.displayOrder())));
        buffer.append(QString(",EXTERNAL_FLAG=%1").arg(account.isExternal() ? "1" : "0"));
        buffer.append(QString(",CLOSED_FLAG=%1").arg(account.isClosed() ? "1" : "0"));
        buffer.append(QString(",ORG='%1'").arg(account.org().trimmed()));

        QString stmt = AccountsSql::UpdateStmt.arg(buffer, account.key());

        qDebug() << stmt;

        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        if(!db.open())
        {
            message = QString("Update record failed: Database failed to open, " + QString(db.lastError().databaseText()));
            return false;
        }

        QSqlQuery q(db);
        if(!q.exec(stmt))
        {
            message = QString("Update record failed: " + q.lastError().text() + "\n\nSQL:" + stmt);
            return false;
        }
        else
        {
            return true;
        }
    }

    static bool deleteAccount(const Account& account, QString& message)
    {
        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        if(!db.open())
        {
            message = QString("Delete record failed: Database failed to open, " + QString(db.lastError().databaseText()));
            return false;
        }
        //make sure the record exists, if it doesnt add/update records could be in change queue only
        int existingCount = 0;
        QString existstmt = AccountsSql::ExistsStmt.arg(account.key());
        QSqlQuery exists(db);
        if(!exists.exec(existstmt))
        {
            message = QString("Delete record failed: " + QString(exists.lastError().databaseText()) + "\n\nExisting record check failure");
            return false;
        }

        if(exists.first())
        {
            existingCount = exists.value(0).toInt();
        }

        if(existingCount == 0)
        {
            //if there is no record in the db it is essentially deleted,
            //return true so that table and change queue are purged of prior records
            return true;
        }

        // make sure account is deletable (closed and has no sub accounts)
        QString deletableStmt = AccountsSql::DeletableCheckStmt.arg(account.key());
        QSqlQuery deletable(db);
        if(!deletable.exec(deletableStmt))
        {
            message = QString("Delete record failed: " + QString(exists.lastError().databaseText()) + "\n\nAccount status check failure");
            return false;
        }

        if(deletable.first())
        {
            QString key = deletable.value(0).toString();
            int closed = deletable.value(1).toInt();
            int subAccounts = deletable.value(2).toInt();

            if(closed ==0 || subAccounts > 0)
            {
                message = QString("Delete record failed: " + QString(exists.lastError().databaseText()) + "\n\nAccount is either closed, or has sub accounts.\n function not allowed");
                return false;
            }
        }
        else
        {
            message = QString("Delete record failed: " + QString(exists.lastError().databaseText()) + "\n\nAccount status check failure, could not identify account");
            return false;
        }

        //if reached this far, account can be deleted, do so.

        QString stmt = AccountsSql::DeleteStmt.arg(account.key());

        QSqlQuery del(db);
        if(!del.exec(stmt))
        {
            message = QString("Delete record failed: " + QString(del.lastError().databaseText()) + "\n\n SQL: " + stmt);
            return false;
        }
        else
        {
            return true;
        }
    }

    static bool unassociatedSourceAccounts(QList<QJsonObject>& fill, QString& message)
    {
        QSqlDatabase db = QSqlDatabase::database("DATABASE");
        if(!db.open())
        {
            message = QString("Database failed to open, " + QString(db.lastError().databaseText()));
            return false;
        }

        QSqlQuery query(db);
        if(!query.exec(AccountsSql::UnassociatedSourceAccountsStmt))
        {
            message = QString("Query failed, " + QString(query.lastError().databaseText()));
            return false;
        }

        while(query.next())
        {
            QJsonObject rec;
            rec["sourcekey"] = query.value(0).toString();
            rec["name"] = query.value(1).toString();
            fill.append(rec);
        }
        return true;
    }

    HierarchyItem * process(const QMap<QString, QList<QString>>& map, const QString& key)
    {
        HierarchyItem *item = new HierarchyItem(key);
        if(map.contains(key))
        {
            QList<QString> subs = map[key];
            for(QString subkey: subs)
            {
                process(map, subkey, item);
            }
        }
        return item;
    }

    void process(const QMap<QString, QList<QString>>& map, const QString& key,HierarchyItem* parent)
    {
        HierarchyItem* item = new HierarchyItem(key);

        if(parent) parent->addSubItem(item);

        //identify children
        if(map.contains(key))
        {
            QList<QString> subs = map[key];
            for(QString subkey: subs)
            {
                process(map, subkey, item);
            }
        }
    }

    HierarchyItem* getAccountRefItem(const QModelIndex& index)
    {
        if(index.isValid())
        {
            HierarchyItem *item = static_cast<HierarchyItem*>(index.internalPointer());
            if(item)
            {
                return item;
            }
        }
        return _root;
    }

    HierarchyItem *_root;
    QMap<QString, std::shared_ptr<Account>> _accounts;
};

AccountsModel::AccountsModel(QObject *parent) : QAbstractItemModel(parent) , impl(new AccountsModelImpl)
{}

AccountsModel::~AccountsModel()
{}

int AccountsModel::rowCount(const QModelIndex &parent) const
{
    HierarchyItem* acct = impl->getAccountRefItem(parent);
    return acct? acct->subItemCount() : 0;
}

int AccountsModel::columnCount(const QModelIndex &parent) const
{
    return 6;
}

QVariant AccountsModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && role == Qt::DisplayRole)
    {
        HierarchyItem* ref = impl->getAccountRefItem(index);
        if(ref)
        {
            auto acct = impl->_accounts[ref->id()];
            if(acct)
            {
                switch (index.column())
                {
                    case 0:
                        return acct->name();
                    case 1:
                        return acct->isClosed();
                    case 2:
                        return acct->isExternal();
                    case 3:
                        return acct->isReported();
                    case 4:
                        return acct->isRollup();
                    case 5:
                        return acct->org();
                    //case 6:
                    //    return acct->sourceKey();
                    default:
                        QVariant();
                }
            }
        }
    }
    return QVariant();
}

QVariant AccountsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch(section)
        {
            case 0:
                return "Account Name";
            case 1:
                return "Closed";
            case 2:
                return "External";
            case 3:
                return "Reported";
            case 4:
                return "Rollup";
            case 5:
                return "Organization";
            //case 6:
            //    return "Source ID";
            default:
                return QVariant();
        }
    }
    return QVariant();
}

Qt::ItemFlags AccountsModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex AccountsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    HierarchyItem* parentAcct = impl->getAccountRefItem(parent);
    if (!parentAcct)
        return QModelIndex();

    HierarchyItem* subAcct = parentAcct->subItem(row);
    if (subAcct)
        return createIndex(row, column, subAcct);

    return QModelIndex();
}

QModelIndex AccountsModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    HierarchyItem *subAcct = impl->getAccountRefItem(child);
    HierarchyItem *parentAcct = subAcct ? subAcct->parent() : nullptr;

    if (parentAcct == impl->_root || !parentAcct)
        return QModelIndex();

    return createIndex(parentAcct->subItemIndex(), 0, parentAcct);
}

std::shared_ptr<Account> AccountsModel::getAccount(const QModelIndex &index) const
{
    if(index.isValid())
    {
        HierarchyItem *item = static_cast<HierarchyItem*>(index.internalPointer());
        if(item && impl->_accounts.contains(item->id()))
        {
            return impl->_accounts[item->id()];
        }
    }
    return impl->_accounts[impl->_root->id()];
}

void AccountsModel::addAccount(const Account &account, const QModelIndex &parent)
{
    HierarchyItem *parentItem = impl->getAccountRefItem(parent);
    std::shared_ptr<Account> acct = std::shared_ptr<Account>(new Account(account));
    acct->setParent(parentItem->id());

    QString msg;

    if(AccountsModelImpl::insertAccount(account, msg))
    {
        beginInsertRows(parent, parentItem->subItemCount(), parentItem->subItemCount());
        HierarchyItem *acctItem = new HierarchyItem(acct->key());
        parentItem->addSubItem(acctItem);
        impl->_accounts.insert(acct->key(), acct);
        endInsertRows();
    }
    else
    {
        //reload from db to reset state to prior to wipe any differences from model
        //raise error
        load();
        ObjectError err(msg, static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
        err.raise();
    }
}

void AccountsModel::updateAccount(const QModelIndex &index)
{
    QString msg;
    std::shared_ptr<Account> acct = getAccount(index);
    if(acct)
    {
        if(AccountsModelImpl::updateAccount(*(acct.get()),msg))
        {
            emit dataChanged(index, index);
            return;
        }
    }
    else
    {
        msg = "Invalid Index";
    }

    //reload from db to reset state to prior to wipe any differences from model
    //raise error
    load();
    ObjectError err(msg, static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
    err.raise();
}

void AccountsModel::moveAccount(const QModelIndex &index, const QModelIndex &parent)
{
    HierarchyItem *item = impl->getAccountRefItem(index);
    HierarchyItem *newParent = impl->getAccountRefItem(parent);
    HierarchyItem *curParent = impl->getAccountRefItem(index.parent());
    std::shared_ptr<Account> account = getAccount(index);

    QString msg;
    if(account)
    {
        account->setParent(newParent->id());
        if(AccountsModelImpl::updateAccount(*(account.get()),msg))
        {
            beginMoveRows(index.parent(), item->subItemIndex(), item->subItemIndex(), parent, newParent->subItemCount());
            curParent->removeSubItem(item->subItemIndex());
            newParent->addSubItem(item);
            endMoveRows();
            return;
        }
    }
    else
    {
        msg = "Invalid Index";
    }

    //reload from db to reset state to prior to wipe any differences from model
    //raise error
    load();
    ObjectError err(msg, static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
    err.raise();
}

void AccountsModel::deleteAccount(const QModelIndex &index)
{
    QString msg;
    HierarchyItem *item = impl->getAccountRefItem(index);
    HierarchyItem *parent = impl->getAccountRefItem(index.parent());
    std::shared_ptr<Account> acct = getAccount(index);

    if(item && item->subItemCount() == 0)
    {
        if(acct && acct->isClosed() && AccountsModelImpl::deleteAccount(*(acct.get()), msg))
        {
            beginRemoveRows(index.parent(), item->subItemIndex(), item->subItemIndex());
            parent->removeSubItem(item->subItemIndex());
            impl->_accounts.remove(acct->key());
            endRemoveRows();
            return;
        }
        else
        {
            msg = "Invalid operation, account must be closed to be deleted";
        }
    }
    else
    {
        msg ="Invalid operation, account must have no sub accounts to be deleted";
    }

    //reload from db to reset state to prior to wipe any differences from model
    //raise error
    load();
    ObjectError err(msg, static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
    err.raise();
}

void AccountsModel::load()
{
    beginResetModel();
    impl->load();
    endResetModel();
}

bool AccountsModel::isRootAccount(std::shared_ptr<Account> account)
{
    if(account->key().compare(Accounts::RootAccountKey, Qt::CaseSensitive) == 0)
    {
        return true;
    }
    return false;
}

QList<QJsonObject> AccountsModel::getUnassociatedSourceAccounts()
{
    QList<QJsonObject> out;
    QString msg;

    if(!impl->unassociatedSourceAccounts(out,msg))
    {
        ObjectError err(msg, static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
        err.raise();
        return QList<QJsonObject>();
    }
    else
    {
        return out;
    }
}


// Proxy
AccountsProxyModel::AccountsProxyModel(QObject *parent) : QSortFilterProxyModel(parent){}

AccountsProxyModel::~AccountsProxyModel(){}

void AccountsProxyModel::setModelIndexFilter(const QModelIndex &index)
{
    this->contextFilter = index;
    invalidateFilter();
}

bool AccountsProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if(contextFilter.isValid() && contextFilter.parent() == source_parent && contextFilter.row() == source_row)
    {
        return false;
    }
    return true;
}

void AccountsProxyModel::setAccountClosedViewFilter(bool showClosed)
{
    this->showClosedAccounts = showClosed;
    invalidateFilter();
}
