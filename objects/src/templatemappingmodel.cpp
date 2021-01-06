#include "objects/templatemappingmodel.h"

struct TemplateMappingModel::TemplateMappingModelImpl
{
    QJsonObject _schema;
    QMap<QString,QString> _map;
    bool _editEnabled;
};

// Model
TemplateMappingModel::TemplateMappingModel(QObject *parent) : QAbstractTableModel(parent), impl(new TemplateMappingModelImpl)
{
    impl->_editEnabled = false;
}

TemplateMappingModel::~TemplateMappingModel()
{}

int TemplateMappingModel::rowCount(const QModelIndex &parent) const
{
    return impl->_map.size();
}

int TemplateMappingModel::columnCount(const QModelIndex &parent) const
{
    return 2; //fixed can only ever be 2 field & the mapping logic
}

QVariant TemplateMappingModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole && index.row() < impl->_map.size())
    {
        QString key = impl->_map.keys().at(index.row());

        switch(index.column())
        {
            case 0:
                return key;
            case 1:
                return impl->_map[key];
            default:
                return QVariant();
        }
    }
    return QVariant();
}

QVariant TemplateMappingModel::headerData(int section, Qt::Orientation orientation, int role) const
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
                    return QString("Database Field");
                case 1:
                    return QString("Mapping");
                default:
                    return QVariant();
            }
        }
        case Qt::Vertical:
        {
            return section + 1;
        }
        default:
            return QVariant();
    }
}

bool TemplateMappingModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.row() < impl->_map.size())
    {
        switch(index.column())
        {
            case 1:
            {
                if(role == Qt::UserRole)
                {
                    QString key = impl->_map.keys().at(index.row());
                    impl->_map[key]= value.toString();
                    this->dataChanged(index, index);
                    return true;
                }
            }
            default:
            {
                return false;
            }
        }
    }
    return false;
}

Qt::ItemFlags TemplateMappingModel::flags(const QModelIndex &index) const
{
    //only the mapping logic field is editable
    if(index.column() == 1 && impl->_editEnabled)
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
}

const QMap<QString, QString> &TemplateMappingModel::getMap() const
{
    return impl->_map;
}

void TemplateMappingModel::loadMapping(QMap<QString, QString> map)
{
    beginResetModel();
    impl->_map = map;
    endResetModel();
}

void TemplateMappingModel::enableEditing(bool enabled)
{
    impl->_editEnabled = enabled;
}

//Delegate

TemplateMappingDelegate::TemplateMappingDelegate(QObject *parent) : QItemDelegate(parent)
{}

QWidget *TemplateMappingDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                               const QModelIndex &index) const
{
    switch(index.column())
    {
        case 1:
        {
            return new QLineEdit(parent);
        }
        default:
            return Q_NULLPTR;

    }

}

void TemplateMappingDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    switch(index.column())
    {
        case 1:
        {
            QString val = index.model()->data(index, Qt::DisplayRole).toString();
            QLineEdit *ctrl = static_cast<QLineEdit *>(editor);
            ctrl->setText(val);
            break;
        }
        default:
            break;
    }
}

void TemplateMappingDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    switch(index.column())
    {
        case 1:
        {
            QLineEdit *control = static_cast<QLineEdit *>(editor);
            QString val = control->text();
            model->setData(index,val, Qt::UserRole);
            break;
        }
        default:
            break;
    }
}

void TemplateMappingDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                                   const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
