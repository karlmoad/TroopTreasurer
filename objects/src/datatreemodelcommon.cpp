#include "objects/datatreemodelcommon.h"


class DataTreeModelCommon::DataTreeModelCommonImpl
{
public:
    DataTreeModelCommonImpl(const QString& rootKey, const QString& keyField, const QString& parentField):
                    _root(new HierarchyItem(rootKey)), _keyField(keyField), _parentField(parentField)
    {}

    ~DataTreeModelCommonImpl()
    {
        delete _root;
    }

    HierarchyItem* const getRoot() const
    {
        return _root;
    }

    QJsonObject getItem(const HierarchyItem& item) const
    {
        if(_data.contains(item.id()))
        {
            return _data.value(item.id());
        }
        return QJsonObject();
    }

    void init(const QList<QJsonObject> &data)
    {
        QMap<QString, QList<QString>> parent2ChildRef;
        for(QJsonObject object: data)
        {
            if(object.contains(_keyField) && object.contains(_parentField))
            {
                _data.insert(object.value(_keyField).toString(), object);

                if (!parent2ChildRef.contains(object.value(_parentField).toString()))
                {
                    parent2ChildRef.insert(object.value(_parentField).toString(), QList<QString>());
                }
                parent2ChildRef[object.value(_parentField).toString()].append(object.value(_keyField).toString());
            }
        }
        process(parent2ChildRef);
    }

    void addHeader(const QString &field, const QString &text)
    {
        QJsonObject header;
        header["field"] = field;
        header["text"] = text;
        _headers.append(header);
    }

    int recordCount()
    {
        return _data.count();
    }

    int headerCount()
    {
        return _headers.count();
    }

    QJsonObject getHeader(int index) const
    {
        if(index < _headers.count())
        {
            return _headers.at(index);
        }
        return QJsonObject();
    }

    void addColumn(const QString &field, const QString &header, const QList<QJsonObject>& data)
    {
        QJsonObject col;
        col["field"] = field;
        col["text"] = header;
        _headers.append(col);

        for(QJsonObject rec: data)
        {
            if(rec.contains("key") && rec.contains(field))
            {
                if(_data.contains(rec.value("key").toString()))
                {
                    QJsonObject rd = _data.value(rec.value("key").toString());
                    rd.insert(field, rec.value(field));
                    _data[rec.value("key").toString()] = rd;
                }
            }
        }
    }

    void addRecord(const QJsonObject &record)
    {
        if(record.contains(_keyField) && record.contains(_parentField))
        {
            HierarchyItem *parent = find(record.value(_parentField).toString(), _root);
            if(parent)
            {
                HierarchyItem *item = new HierarchyItem(record.value(_keyField).toString());
                parent->addSubItem(item);
                _data.insert(record.value(_keyField).toString(), record);
            }
        }
    }

private:
    QString _keyField;
    QString _parentField;
    HierarchyItem *_root;
    QList<QJsonObject> _headers;
    QMap<QString, QJsonObject> _data;

    HierarchyItem* find(const QString& key, HierarchyItem* item)
    {
        if(item->id().compare(key, Qt::CaseSensitive) == 0)
        {
            return item;
        }

        if(item->subItemCount() > 0)
        {
            for(int i=0; i< item->subItemCount(); i++)
            {
                HierarchyItem* f = find(key, item->subItem(i));
                if(f)
                {
                    return f;
                }
            }
        }
        return nullptr;
    }

    void process(const QMap<QString, QList<QString>>& map)
    {
        if(map.contains(_root->id()))
        {
            QList<QString> subs = map.value(_root->id());
            for(QString subkey: subs)
            {
                process(map, subkey, _root);
            }
        }
    }

    void process(const QMap<QString, QList<QString>>& map, const QString& key,HierarchyItem* parent)
    {
        HierarchyItem* item = new HierarchyItem(key);

        if(parent) parent->addSubItem(item);

        //identify children
        if(map.contains(key))
        {
            QList<QString> subs = map[key];
            for(QString subkey: subs)
            {
                process(map, subkey, item);
            }
        }
    }
};

DataTreeModelCommon::DataTreeModelCommon(const QString& rootKey, const QString& keyField, const QString& parentField):
                                                impl(new DataTreeModelCommonImpl(rootKey, keyField, parentField))
{}

DataTreeModelCommon::~DataTreeModelCommon()
{}

void DataTreeModelCommon::addRecord(const QJsonObject &record)
{
    impl->addRecord(record);
}

void DataTreeModelCommon::addColumn(const QString &field, const QString &header, const QList<QJsonObject>& data)
{
    impl->addColumn(field, header, data);
}

void DataTreeModelCommon::initialize(const QList<QJsonObject> &data)
{
    impl->init(data);
}

void DataTreeModelCommon::addHeader(const QString &field, const QString &text)
{
    impl->addHeader(field, text);
}

int DataTreeModelCommon::recordCount()
{
    return impl->recordCount();
}

int DataTreeModelCommon::columnCount()
{
    return impl->headerCount();
}

QJsonObject DataTreeModelCommon::getRecord(const HierarchyItem &item)
{
    return impl->getItem(item);
}

QJsonObject DataTreeModelCommon::getHeader(int index)
{
    return impl->getHeader(index);
}

HierarchyItem *DataTreeModelCommon::getHierarchyRef(const QModelIndex &index)
{
    if(index.isValid())
    {
        HierarchyItem *item = static_cast<HierarchyItem*>(index.internalPointer());
        if(item)
        {
            return item;
        }
    }
    return impl->getRoot();
}

bool DataTreeModelCommon::isRoot(const HierarchyItem *item)
{
    return impl->getRoot() == item;
}
