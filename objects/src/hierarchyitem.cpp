#include "objects/hierarchyitem.h"

struct HierarchyItem::HierarchyItemImpl
{
    HierarchyItemImpl(){}
    virtual ~HierarchyItemImpl()
    {
        qDeleteAll(subItems);
    }

    QString id;
    HierarchyItem *parent;
    QList<HierarchyItem*> subItems;
};

HierarchyItem::HierarchyItem(const QString &id): impl(new HierarchyItemImpl)
{
    impl->id = id;
    impl->parent = nullptr;
}

HierarchyItem::~HierarchyItem(){}

QString HierarchyItem::id() const
{
    return impl->id;
}

HierarchyItem *HierarchyItem::parent()
{
    return impl->parent;
}

void HierarchyItem::setParent(HierarchyItem *parent)
{
    impl->parent = parent;
}

int HierarchyItem::subItemCount() const
{
    return impl->subItems.length();
}

HierarchyItem *HierarchyItem::subItem(int index)
{
    if(index >= 0 && index < impl->subItems.length())
    {
        return impl->subItems.at(index);
    }
    return nullptr;
}

HierarchyItem *HierarchyItem::removeSubItem(int pos)
{
    if(pos >= 0 && pos < impl->subItems.length())
    {
        HierarchyItem* sub = impl->subItems.takeAt(pos);
        sub->setParent(nullptr);
        return sub;
    }
}

void HierarchyItem::addSubItem(HierarchyItem *sub)
{
    sub->setParent(this);
    impl->subItems.append(sub);
}

int HierarchyItem::subItemIndex() const
{
    if(impl->parent)
    {
        return impl->parent->impl->subItems.indexOf(const_cast<HierarchyItem*>(this));
    }
    return 0;
}

int HierarchyItem::indexOfSubItem(const QString &id)
{
    for(int i =0; i<impl->subItems.length(); ++i)
    {
        if(impl->subItems[i]->id().compare(id, Qt::CaseSensitive) == 0)
            return i;
    }
    return 0;
}
