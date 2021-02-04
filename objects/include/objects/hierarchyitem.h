#ifndef TROOPTREASURER_HIERARCHYITEM_H
#define TROOPTREASURER_HIERARCHYITEM_H

#include <QString>
#include <QList>
#include <memory>

class HierarchyItem
{
public:
    explicit HierarchyItem(const QString& id);
    ~HierarchyItem();

    QString id() const;
    HierarchyItem* parent();
    void setParent(HierarchyItem *parent= nullptr);
    int subItemCount() const;
    HierarchyItem* subItem(int index);
    HierarchyItem* removeSubItem(int pos);
    void addSubItem(HierarchyItem *sub= nullptr);
    int subItemIndex() const;
    int indexOfSubItem(const QString& id);

private:
    class HierarchyItemImpl;
    std::shared_ptr<HierarchyItemImpl> impl;
};


#endif //TROOPTREASURER_HIERARCHYITEM_H
