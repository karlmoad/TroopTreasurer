#ifndef TABLEFIELD_H
#define TABLEFIELD_H

#include "informationschema.h"
#include <QVariant>

class TableField: public InformationSchema
{
public:
    enum class DataType{NONE=0, STRING, INT, DECIMAL, BOOLEAN, TEXT};

public:
    TableField();
    virtual ~TableField();

    DataType getDataType() const;
    QString getName() const;
    QString getFormat() const;
    bool isKey() const;
    bool isMatchKey() const;
    bool isNullable() const;
    QVariant getDefaultValue() const;
    int getLength() const;

    virtual QJsonObject toJson() override;
    virtual bool isNull() const override;

private:
    virtual void initialize(const QJsonObject &json) override;

private:
    class TableFieldImpl;
    std::shared_ptr<TableFieldImpl> impl;
};


#endif //TABLEFIELD_H
