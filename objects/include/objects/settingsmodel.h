/*
 * TroopTreasurer
 * Author: Karl Moad
 * ---------------------------------
 * SettingsModel - Provides a model for interaction
 * with a defined set of settings per a configuration
 * specification
*/

#ifndef TROOPTREASURER_SETTINGSMODEL_H
#define TROOPTREASURER_SETTINGSMODEL_H

#include <QtWidgets>
#include <QAbstractTableModel>
#include <QStyledItemDelegate>
#include <QVariant>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

class SettingsModel: public QAbstractTableModel
{
Q_OBJECT
public:
    explicit SettingsModel(QObject *parent= nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void loadSettings(const QJsonArray& meta, const QJsonObject& settings);
    const QJsonObject& getSettings() const;

private:
    QJsonObject _settingsValues;
    QJsonArray _settingsMeta;
};

class SettingsModelDelegate: public QStyledItemDelegate
{
Q_OBJECT
public:
    explicit SettingsModelDelegate(QObject *parent = 0);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    enum EditorType {TEXT,FILE,FLAG,PWD,UNKNOWN};
    static EditorType getEditorType(const QString& value);
};

#endif //TROOPTREASURER_SETTINGSMODEL_H
