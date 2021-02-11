#ifndef TROOPTREASURER_DATATABLEMODELIMPL_H
#define TROOPTREASURER_DATATABLEMODELIMPL_H

#include <QVector>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class DataTableModelImpl
{
public:
    DataTableModelImpl(): _displayVerticalColumns(false), _displayHorizontalColumns(false) {};
    virtual ~DataTableModelImpl()=default;

    int count()
    {
        return _data.count();
    }

    int columnCount()
    {
        return _headers.count();
    }

    void setVeritcalColumnsEnabled(bool enabled=true)
    {
        _displayVerticalColumns = enabled;
    }

    void setHorizontalColumnsEnabled(bool enabled=true)
    {
        _displayHorizontalColumns = enabled;
    }

    bool isVerticalColumnsEnabled()
    {
        return _displayVerticalColumns;
    }

    bool isHorizontalColumnsEnabled()
    {
        return _displayHorizontalColumns;
    }

    std::shared_ptr<QJsonObject> getHeader(int section)
    {
        if(section < _headers.length())
            return _headers.at(section);
        else
            return nullptr;
    }

    std::shared_ptr<QJsonObject> getRecord(int row)
    {
        if(row < _data.count())
            return _data.at(row);
        else
            return nullptr;
    }

    void addHeader(const QJsonObject& header)
    {
        auto h = std::make_shared<QJsonObject>(header);
        _headers.append(h);
    }

    void addData(const QJsonObject& data)
    {
        auto d = std::make_shared<QJsonObject>(data);
        _data.append(d);
    }

private:
    bool _displayHorizontalColumns;
    bool _displayVerticalColumns;
    QList<std::shared_ptr<QJsonObject>> _headers;
    QVector<std::shared_ptr<QJsonObject>> _data;
};


#endif //TROOPTREASURER_DATATABLEMODELIMPL_H
