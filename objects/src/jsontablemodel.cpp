#include <objects/jsontablemodel.h>


JsonTableModel::JsonTableModel(QObject *parent) : DataTableModel(parent)
{
    impl->setVeritcalColumnsEnabled();
    impl->setHorizontalColumnsEnabled();
}

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
    beginResetModel();
    for(const QJsonObject json : data)
    {
        impl->addData(json);
    }
    endResetModel();
}
