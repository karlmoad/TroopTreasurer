#ifndef TROOPTREASURER_GROUP_H
#define TROOPTREASURER_GROUP_H

#include <QString>
#include <QJsonObject>
#include <memory>

class Group
{
public:
    Group();
    explicit Group(const Group &other);
    virtual ~Group();

    QString key() const;
    QString description() const;
    void setDescription(const QString &value);

    Group& operator=(const Group& other);
    bool operator==(const Group &rhs) const;
    bool operator!=(const Group &rhs) const;
private:
    friend class GroupModel;
    explicit Group(const QJsonObject &json);

    class GroupImpl;
    std::shared_ptr<GroupImpl> impl;
};


#endif //TROOPTREASURER_GROUP_H
