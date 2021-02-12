#ifndef TROOPTREASURER_DATATREEMODELCOMMON_H
#define TROOPTREASURER_DATATREEMODELCOMMON_H

#include <QMap>
#include <QVector>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QModelIndex>
#include <memory>
#include "hierarchyitem.h"

class DataTreeModelCommon
{
public:
    DataTreeModelCommon(const QString& rootKey, const QString& keyField, const QString& parentField);
    virtual ~DataTreeModelCommon();

    void initialize(const QList<QJsonObject>& data);
    void addHeader(const QString& field, const QString& text);
    void addRecord(const QJsonObject& record);
    void addColumn(const QString& field, const QString& headerText, const QList<QJsonObject>& data);

    int recordCount();
    int columnCount();

    HierarchyItem* getHierarchyRef(const QModelIndex& index);
    QJsonObject getRecord(const HierarchyItem& item);
    QJsonObject getHeader(int index);
    bool isRoot(const HierarchyItem* item);

private:
    class DataTreeModelCommonImpl;
    std::shared_ptr<DataTreeModelCommonImpl> impl;

}
#endif //TROOPTREASURER_DATATREEMODELCOMMON_H
