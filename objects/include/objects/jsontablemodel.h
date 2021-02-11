#ifndef TROOPTREASURER_JSONTABLEMODEL_H
#define TROOPTREASURER_JSONTABLEMODEL_H

#include <QList>
#include <QMap>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "objects/datatablemodel.h"

class JsonTableModel: public DataTableModel
{
Q_OBJECT
public:
    explicit JsonTableModel( QObject* parent= nullptr);
    virtual ~JsonTableModel();
    void setHeaders(const QList<QJsonObject> &headers);
    void setData(const QList<QJsonObject> &data);
};

#endif //TROOPTREASURER_JSONTABLEMODEL_H
