/*
 * TroopTreasurer
 * Author: Karl Moad
 * ---------------------------------
 * Template Mapping Model - Data model associated to
 * the editing and display of an import template map
 *
 * Template Mapping Delegate - delegate for
 * a template model
 *
*/

#ifndef TROOPTREASURER_TEMPLATEMAPPINGMODEL_H
#define TROOPTREASURER_TEMPLATEMAPPINGMODEL_H

#include <QAbstractTableModel>
#include <QtWidgets>
#include <QItemDelegate>
#include <QMessageBox>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QList>
#include <memory>

class TemplateMappingModel: public QAbstractTableModel
{
Q_OBJECT
public:
    explicit TemplateMappingModel(QObject *parent=nullptr);
    virtual ~TemplateMappingModel();

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    const QMap<QString, QString>& getMap() const;
    void loadMapping(QMap<QString,QString> map);
    void enableEditing(bool enabled);

private:
    class TemplateMappingModelImpl;
    std::shared_ptr<TemplateMappingModelImpl> impl;
};

class TemplateMappingDelegate: public QItemDelegate
{
Q_OBJECT
public:
    explicit TemplateMappingDelegate(QObject *parent = 0);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif //TROOPTREASURER_TEMPLATEMAPPINGMODEL_H
