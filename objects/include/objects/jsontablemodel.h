#ifndef TROOPTREASURER_JSONTABLEMODEL_H
#define TROOPTREASURER_JSONTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QMap>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <memory>
#include <QDebug>

class JsonTableModel: public QAbstractTableModel
{
Q_OBJECT
public:
    explicit JsonTableModel( QObject* parent= nullptr);
    virtual ~JsonTableModel();
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    void setHeaderMap(const QMap<QString,QString> &headers);
    void setData(const QList<QJsonObject> &data);

protected:
    class JsonTableModelImpl;
    std::shared_ptr<JsonTableModelImpl> impl;
};

#endif //TROOPTREASURER_JSONTABLEMODEL_H
