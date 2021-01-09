/*
 * TroopTreasurer
 * Author: Karl Moad
 * ---------------------------------
 * CSV Model - Data model associated to
 * the display of Comma Separated Value file data
 *
 *
*/

#ifndef TROOPTREASURER_CSVMODEL_H
#define TROOPTREASURER_CSVMODEL_H

#include <QAbstractTableModel>
#include <QAbstractItemModel>
#include <QFile>
#include <QList>
#include <QString>
#include <QDebug>
#include <memory>
#include "objects/objecterror.h"

class CSVModel: public QAbstractTableModel
{
Q_OBJECT
public:
    explicit CSVModel( QObject* parent= nullptr);
    virtual ~CSVModel();
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    void load(QFile *file,QChar sep=',', bool includesHeader=true);

private:
    class CSVModelImpl;
    std::shared_ptr<CSVModelImpl> impl;

};



#endif //TROOPTREASURER_CSVMODEL_H
