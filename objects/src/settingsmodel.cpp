#include "objects/settingsmodel.h"

SettingsModel::SettingsModel(QObject *parent) : QAbstractTableModel(parent)
{}

int SettingsModel::rowCount(const QModelIndex &parent) const
{
    return _settingsMeta.size();
}

int SettingsModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant SettingsModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < _settingsMeta.size())
    {
        QJsonObject meta = _settingsMeta.at(index.row()).toObject();
        switch(index.column())
        {
            case 0:
            {
                if (role == Qt::DisplayRole)
                    return meta["display"].toString();
                else
                    return QVariant();
            }
            case 1:
            {
                switch(role)
                {
                    case Qt::UserRole:
                    {
                        return meta;
                    }
                    case Qt::DisplayRole:
                    {
                        if (_settingsValues.contains(meta["name"].toString()))
                        {
                            return _settingsValues[meta["name"].toString()].toVariant();
                        }
                        else
                        {
                            return QVariant();
                        }
                    }
                    default:
                        return QVariant();
                }
            }
            default:
                return QVariant();
        }
    }
    return QVariant();
}

QVariant SettingsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( role != Qt::DisplayRole )
    {
        return QVariant();
    }

    switch(orientation)
    {
        case Qt::Horizontal:
        {
            switch(section)
            {
                case 0:
                    return QString("Setting");
                case 1:
                    return QString("Value");
            }
        }
        default:
            return QVariant();
    }
}

bool SettingsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.row() < _settingsMeta.size())
    {
        switch(role)
        {
            case Qt::UserRole:
            {
                QJsonObject meta = _settingsMeta.at(index.row()).toObject();
                _settingsValues[meta["name"].toString()] = QJsonValue::fromVariant(value);
                this->dataChanged(index, index);
                return true;
            }
            default:
            {
                return false;
            }
        }
    }
    return false;
}

Qt::ItemFlags SettingsModel::flags(const QModelIndex &index) const
{
    switch(index.column())
    {
        case 1:
            return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
        default:
            return Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
    }
}

const QJsonObject& SettingsModel::getSettings() const
{
    return _settingsValues;
}

void SettingsModel::loadSettings(const QJsonArray& meta, const QJsonObject& settings)
{
    beginResetModel();
    _settingsMeta = meta;
    _settingsValues = settings;

    //init default values
    for(int i = 0; i< _settingsMeta.size(); i++)
    {
        QJsonObject md = _settingsMeta.at(i).toObject();
        if(md.contains("default"))
        {
            QString name = md["name"].toString();
            QJsonValue val = md[name];
            if(_settingsValues.contains(name))
            {
                if(_settingsValues[name].toString().trimmed().size() <= 0)
                {
                    _settingsValues[name] = val;
                }
            }
            else
            {
                _settingsValues[name] = val;
            }
        }
    }
    endResetModel();
}

SettingsModelDelegate::SettingsModelDelegate(QObject *parent) : QStyledItemDelegate(parent){}

QWidget* SettingsModelDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QJsonObject meta = index.model()->data(index, Qt::UserRole).toJsonObject();
    if(meta.contains("type"))
    {
        EditorType type = SettingsModelDelegate::getEditorType(meta["type"].toString());
        switch(type)
        {
            case FLAG:
            {
                auto editor = new QCheckBox(parent);
                editor->setAutoFillBackground(true);
                return editor;
            }
//            case FILE:
//            {
//                QFileDialog *editor = new QFileDialog(parent);
//                editor->setFileMode(QFileDialog::FileMode::ExistingFile);
//                editor->setAutoFillBackground(true);
//                return editor;
//            }
            case PWD:
            {
                QLineEdit *editor = new QLineEdit(parent);
                //editor->setEchoMode(QLineEdit::EchoMode::Password);
                editor->setAutoFillBackground(true);
                return editor;
            }
            default:
            {
                auto editor = new QLineEdit(parent);
                editor->setAutoFillBackground(true);
                return editor;
            }
        }
    }
    else
    {
        return new QLineEdit(parent);
    }
}

void SettingsModelDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QJsonObject meta = index.model()->data(index, Qt::UserRole).toJsonObject();
    if(meta.contains("type"))
    {
        EditorType type = SettingsModelDelegate::getEditorType(meta["type"].toString());
        switch (type)
        {
            case FLAG:
            {
                QCheckBox *ctrl = static_cast<QCheckBox *>(editor);
                QVariant val = index.model()->data(index, Qt::DisplayRole);
                if(!val.isNull())
                {
                    if(val.toBool())
                    {
                        ctrl->setCheckState(Qt::Checked);
                        return;
                    }
                }
                ctrl->setCheckState(Qt::Unchecked);
                return;
            }
//            case FILE:
//            {
//                QString def_path = QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation)[0];
//                QVariant val = index.model()->data(index, Qt::DisplayRole);
//                if(!val.isNull())
//                {
//                    QFileDialog *ctrl = static_cast<QFileDialog *>(editor);
//                    QString cur = val.toString();
//                    if(cur.size()>0)
//                    {
//                        QFileInfo fi(cur);
//                        ctrl->setDirectory(fi.dir());
//                        ctrl->selectFile(fi.fileName());
//                        return;
//                    }
//                    else
//                    {
//                        ctrl->setDirectory(def_path);
//                        return;
//                    }
//                }
//            }
            default:
            {
                QVariant val = index.model()->data(index, Qt::DisplayRole);
                if(!val.isNull())
                {
                    QLineEdit *ctrl = static_cast<QLineEdit *>(editor);
                    ctrl->setText(val.toString());
                }
            }
        }
    }
}

void SettingsModelDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QJsonObject meta = index.model()->data(index, Qt::UserRole).toJsonObject();
    QString key = meta["name"].toString();

    EditorType type = (meta.contains("type") ? SettingsModelDelegate::getEditorType(meta["type"].toString()) : SettingsModelDelegate::EditorType::UNKNOWN);
    switch (type)
    {
        case FLAG:
        {
            QCheckBox *ctrl = static_cast<QCheckBox *>(editor);
            bool v = ctrl->isChecked();
            model->setData(index,v, Qt::UserRole);
            return;
            break;
        }
//        case FILE:
//        {
//            QFileDialog *ctrl = static_cast<QFileDialog *>(editor);
//            if(ctrl->selectedFiles().size()>0)
//            {
//                model->setData(index,ctrl->selectedFiles()[0],Qt::UserRole);
//            }
//            break;
//        }
        default:
        {
            QLineEdit *ctrl = static_cast<QLineEdit *>(editor);
            model->setData(index, ctrl->text(), Qt::UserRole);
        }
    }
}

void SettingsModelDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                                 const QModelIndex &index) const
{
//    QJsonObject meta = index.model()->data(index, Qt::UserRole).toJsonObject();
//    if(meta.contains("type"))
//    {
//        EditorType type = SettingsModelDelegate::getEditorType(meta["type"].toString());
//        if(type == FILE)
//        {
//            QRect r = option.rect;
//            r.setHeight(600);
//            r.setWidth(600);
//            editor->setGeometry(r);
//            return;
//        }
//    }
    editor->setGeometry(option.rect);
}

SettingsModelDelegate::EditorType SettingsModelDelegate::getEditorType(const QString &value)
{
    //if(value.compare("",Qt::CaseSensitive)==0)

    if(value.compare("text",Qt::CaseSensitive) == 0)
        return SettingsModelDelegate::EditorType::TEXT;
    if(value.compare("password", Qt::CaseSensitive) ==0)
        return SettingsModelDelegate::EditorType::PWD;
    if(value.compare("flag",Qt::CaseSensitive)==0)
        return SettingsModelDelegate::EditorType::FLAG;
    if(value.compare("file",Qt::CaseSensitive)==0)
        return SettingsModelDelegate::EditorType::FILE;
    return SettingsModelDelegate::EditorType::UNKNOWN;
}

