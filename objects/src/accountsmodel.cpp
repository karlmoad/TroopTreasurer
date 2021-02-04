#include "objects/accountsmodel.h"
#include <QUuid>
#include <QMap>
#include <QQueue>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include "objects/hierarchyitem.h"
#include <QDebug>

class AccountsModel::AccountsModelImpl
{
public:
    void load()
    {
//        QMap<QString, QList<QString>> parent2ChildRef;
//        QJsonObject data;
//        QFile file(":/resources/items.json");
//        if(file.open(QFile::ReadOnly | QFile::Text)){
//            QByteArray raw = file.readAll();
//            file.close();
//            data = QJsonDocument::fromJson(raw).object();
//        }
//
//        QJsonArray items = data["items"].toArray();
//
//
//        auto rootAcct = std::shared_ptr<Account>(new Account);
//        rootAcct->setKey(_rootKey);
//        rootAcct->setName("Root Account");
//        rootAcct->setReported(true);
//        rootAcct->setRollup(true);
//        rootAcct->setSourceKey("--root--");
//        _accounts.insert(rootAcct->key(), rootAcct);
//
//        for(int i =0; i<items.size(); ++i)
//        {
//            QJsonObject o = items[i].toObject();
//            auto acct = std::shared_ptr<Account>(new Account);
//            acct->setKey(o["id"].toString());
//            acct->setName(o["name"].toString());
//            acct->setOrg(o["org"].toString());
//            acct->setSourceKey(o["source"].toString());
//            acct->setParent(o["parent"].toString());
//            acct->setRollup(o["rollup"].toBool());
//            acct->setExternal(o["external"].toBool());
//            acct->setClosed(o["closed"].toBool());
//            acct->setReported(o["reported"].toBool());
//            _accounts.insert(acct->key(), acct);
//
//            if (!parent2ChildRef.contains(acct->parent()))
//            {
//                parent2ChildRef.insert(acct->parent(), QList<QString>());
//            }
//            parent2ChildRef[acct->parent()].append(acct->key());
//        }
//
//        qDebug() << parent2ChildRef;
//        qDebug() << _accounts.keys();
//
//        _root = process(parent2ChildRef, _rootKey);
//
//        debug();
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
    const QString _rootKey = "{00000000-0000-0000-0000-000000000000}";
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
    return 7;
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
                    case 6:
                        return acct->sourceKey();
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
            case 6:
                return "Source ID";
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

    beginInsertRows(parent, parentItem->subItemCount(),parentItem->subItemCount());

    acct->setParent(parentItem->id());
    HierarchyItem *acctItem = new HierarchyItem(acct->key());
    parentItem->addSubItem(acctItem);
    impl->_accounts.insert(acct->key(), acct);

    endInsertRows();
}

void AccountsModel::updateAccount(const QModelIndex &index)
{
    emit dataChanged(index,index);
}

void AccountsModel::moveAccount(const QModelIndex &index, const QModelIndex &parent)
{
    HierarchyItem *item = impl->getAccountRefItem(index);
    HierarchyItem *newParent = impl->getAccountRefItem(parent);
    HierarchyItem *curParent = impl->getAccountRefItem(index.parent());
    std::shared_ptr<Account> account = getAccount(index);

    beginMoveRows(index.parent(), item->subItemIndex(), item->subItemIndex(), parent, newParent->subItemCount());

    curParent->removeSubItem(item->subItemIndex());
    newParent->addSubItem(item);
    account->setParent(newParent->id());

    endMoveRows();
}

void AccountsModel::deleteAccount(const QModelIndex &index)
{

}

void AccountsModel::load()
{
    beginResetModel();
    impl->load();
    endResetModel();
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
