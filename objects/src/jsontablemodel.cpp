#include <objects/jsontablemodel.h>


JsonTableModel::JsonTableModel(QObject *parent) : DataTableModel(parent){}

JsonTableModel::~JsonTableModel(){}

void JsonTableModel::setHeaders(const QList<QJsonObject> &headers)
{
    for(const QJsonObject json : headers)
    {
        impl->addHeader(json);
    }
}

void JsonTableModel::setData(const QList <QJsonObject>& data)
{
    for(const QJsonObject json : data)
    {
        impl->addData(json);
    }
}
