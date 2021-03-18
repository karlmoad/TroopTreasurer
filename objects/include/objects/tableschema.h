#ifndef TABLESCHEMA_H
#define TABLESCHEMA_H

#include <memory>
#include <bitset>
#include "informationschema.h"

class RelationshipReference;
class TableField;

class ImportOptions
{
public:

    bool isTruncateEnabled() const {return _state[0];}
    bool isUpdateEnabled() const {return _state[1];}
    bool isDupCheckEnabled() const {return _state[2];}
    bool isAppendEnabled() const {return _state[3];}
    bool isArchiveEnabled() const {return _state[4];}

    void setTruncateEnabled(bool state) {_state[0] = state;}
    void setUpdateEnabled(bool state) {_state[1] = state;}
    void setDupCheckEnabled(bool state) {_state[2] = state;}
    void setAppendEnabled(bool state) {_state[3] = state;}
    void setArchiveEnabled(bool state) (_state[4] = state;}

private:
    std::bitset<5> _state;
};

class TableSchema: public InformationSchema
{
public:
    TableSchema();
    virtual ~TableSchema();

    std::shared_ptr<TableField> getField(const QString& ref) const;
    QMap<QString,std::shared_ptr<TableField>> getFields() const;

    std::shared_ptr<RelationshipReference> getRelationshipReference(const QString& name) const;
    QMap<QString,std::shared_ptr<RelationshipReference>> getRelationshipReferences() const;

    bool isImportSupported() const;
    ImportOptions importOptions() const;
    bool isArchiveSupported() const;
    QString archiveTable() const;

    virtual QJsonObject toJson() override;
    virtual bool isNull() const override;
    virtual void initialize(const QJsonObject &json) override;

private:
    class TableSchemaImpl;
    std::shared_ptr<TableSchemaImpl> impl;
};


#endif //TABLESCHEMA_H
