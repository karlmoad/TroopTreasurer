#include "objects/databasecontroller.h"
#include "objects/tablefield.h"

class DatabaseController::DatabaseControllerImpl
{
public:
    DatabaseControllerImpl(std::shared_ptr<TableSchema> table, DataResponsibility responsibility): _table(table), _responsibility(responsibility)
    {
        // init mapping
        if(_table->isBindingSupported())
        {
            auto refs = _table->getMapping();
            for(Reference r : refs)
            {
                _object2FieldMap.insert(r.getName(), r.getTarget());
                _field2ObjectMap.insert(r.getTarget(), r.getName());
            }
        }
    }


    ~DatabaseControllerImpl()=default;

    void setArguments(const QList<Criterion>& args)
    {
        _args = args;
    }

    void addRelationship(std::shared_ptr<Relationship> relationship, const QString &fieldName, const QString &fieldValue)
    {
        _relationships[fieldName] = relationship;
        _relationshipValues[fieldName] = fieldValue;
    }

    ResultStatus load(QList<QJsonObject> & results)
    {
        return ResultStatus();
    }

    ResultStatus add(const QJsonObject &json)
    {
        return ResultStatus();
    }

    ResultStatus update(const QJsonObject &json)
    {
        return ResultStatus();
    }

    ResultStatus remove(const QJsonObject &json)
    {
        return ResultStatus();
    }

private:

    QString generateSelectStatement()
    {
        QString statement;
        if(_table && _table->isBindingSupported())
        {
            auto fields = _table->getFields();
            auto keys = fields.keys();
            QString selectFields;
            for(int i = 0; i< keys.count(); i++)
            {
                if(i > 0)
                {
                    selectFields.append(" ,");
                }
                selectFields.append(QString("%1.%2").arg(_table->tableName(), fields[keys[i]]->getName()));
            }

            QString selectFrom = _table->tableName();
            QString selectWhere;
            if(_relationships.count() > 0)
            {
                for(QString relation : _relationships.keys())
                {
                    QString Join = generateJoinCondition(relation, _table->tableName());
                    if(!Join.isEmpty())
                    {
                        selectFrom.append(" " + Join);
                    }
                }
            }


            if(_args.count() > 0)
            {
                auto argsStmt = generateArgsCriteria(_table->tableName());
                if(!argsStmt.isEmpty() && !argsStmt.isNull())
                {
                    selectWhere = argsStmt;
                }
            }





        }
        return statement;
    }

    QString generateJoinCondition(const QString &relationship, const QString& table)
    {
        QString output;
        if(_table && _relationships.contains(relationship))
        {
            std::shared_ptr<Relationship> rel = _relationships[relationship];
            std::shared_ptr<RelationshipMapping> ref = _table->getRelationshipReference(_relationships[relationship]->name());
            if(rel && ref)
            {
                auto joinMap = ref->getMapping();
                if(joinMap.count() > 0)
                {
                    output = QString("JOIN %1").arg(rel->getTableName());
                    for(int i = 0; i < joinMap.count(); i++)
                    {
                        auto joinItem = joinMap.at(i);
                        QString joinOn = QString(" ON %1.%2 = %3.%4").arg(table, joinItem->getName(), rel->getTableName(), joinItem->getTarget());

                        if(i > 0)
                        {
                            output.append(" AND");
                        }

                        output.append(joinOn);
                    }
                }
            }
        }
        return output;
    }

    QString generateArgsCriteria(const QString& table)
    {
        QString out;
        int i = 0;
        for(Criterion c : _args)
        {
            auto field = _table->getField(c.getName());
            if(field)
            {
                auto critStmt = formatStatementCriterion(table, field, c.getOperator(), c.getValues());
                if(!critStmt.isEmpty() && !critStmt.isNull())
                {
                    if (i > 0)
                    {
                        out.append(" AND ");
                    }
                    out.append(critStmt);
                }
            }
        }
        return out;
    }

    static QString formatStatementCriterion(const QString &table, std::shared_ptr<TableField> field, Operator op, const QList<QVariant>& values)
    {
        QString out;
        if(field && values.count() > 0)
        {
            switch (op)
            {
                case Operator::BEGINS_WITH:
                case Operator::ENDS_WITH:
                case Operator::CONTAINS:
                {
                    out = QString("%1.%2 LIKE %3").arg(table,field->getName(), formatValueForSQL(field,op,values.at(0)));
                    break;
                }
                case Operator::IN:
                {
                    QString vals;
                    for(int i = 0; i <values.count(); i++)
                    {
                        if(i > 0)
                        {
                            vals.append(",");
                        }
                        vals.append(formatValueForSQL(field, op, values.at(i)));
                    }
                    out = QString("%1.%2 IN (%3)").arg(table,field->getName(), vals);
                    break;
                }
                case Operator::BETWEEN:
                {
                    if(values.count() >= 2)
                    {
                        out = QString("%1.%2 BETWEEN %3 AND %4").arg(table, field->getName(), formatValueForSQL(field, op, values.at(0)), formatValueForSQL(field, op, values.at(1)));
                    }
                    break;
                }
                case Operator::GREATER:
                {
                    out = QString("%1.%2 > %3").arg(table,field->getName(), formatValueForSQL(field,op,values.at(0)));
                    break;
                }
                case Operator::LESS:
                {
                    out = QString("%1.%2 < %3").arg(table,field->getName(), formatValueForSQL(field,op,values.at(0)));
                    break;
                }
                case Operator::LESS_EQUAL:
                {
                    out = QString("%1.%2 <= %3").arg(table,field->getName(), formatValueForSQL(field,op,values.at(0)));
                    break;
                }
                case Operator::GREATER_EQUAL:
                {
                    out = QString("%1.%2 >= %3").arg(table,field->getName(), formatValueForSQL(field,op,values.at(0)));
                    break;
                }
                default: //equals
                {
                    out = QString("%1.%2 = %3").arg(table,field->getName(), formatValueForSQL(field,op,values.at(0)));
                    break;
                }
            }
        }
        return out;
    }

    static QString formatValueForSQL(std::shared_ptr<TableField> field, Operator op, const QVariant& value)
    {
        switch (field->getDataType())
        {
            case TableField::DataType::INT:
            case TableField::DataType::DECIMAL:
            case TableField::DataType::NONE:
            {
                return value.toString();
            }
            case TableField::DataType::BOOLEAN:
            {
                if(value.toBool())
                {
                    return "1";
                }
                else
                {
                    return "0";
                }
            }
            default:
            {
                switch (op)
                {
                    case Operator::BEGINS_WITH:
                        return QString("'%%1'").arg(value.toString());

                    case Operator::ENDS_WITH:
                        return QString("'%1%'").arg(value.toString());
                    case Operator::CONTAINS:
                        return QString("'%%1%'").arg(value.toString());
                    default:
                        return QString("'%1'").arg(value.toString());
                }
            }
        }
    }

    QMap<QString,std::shared_ptr<Relationship>> _relationships;
    QMap<QString, QString> _relationshipValues;
    QList<Criterion> _args;
    QMap<QString, QString> _object2FieldMap;
    QMap<QString, QString> _field2ObjectMap;
    std::shared_ptr<TableSchema> _table;
    DataResponsibility _responsibility;
};

DatabaseController::DatabaseController(std::shared_ptr<TableSchema> tableSchema, DataResponsibility responsibility): impl(new DatabaseControllerImpl(tableSchema, responsibility))
{}

DatabaseController::~DatabaseController()
{}

void DatabaseController::addRelationship(std::shared_ptr<Relationship> relationship, const QString &fieldName,
                                         const QString &fieldValue)
{
    impl->addRelationship(relationship, fieldName, fieldValue);
}

void DatabaseController::setArguments(const QList<Criterion>& args)
{
    impl->setArguments(args);
}

ResultStatus DatabaseController::load(QList<QJsonObject> &results)
{
    return impl->load(results);
}

ResultStatus DatabaseController::add(const QJsonObject &jsonObject)
{
    return impl->add(jsonObject);
}

ResultStatus DatabaseController::update(const QJsonObject &jsonObject)
{
    return impl->update(jsonObject);
}

ResultStatus DatabaseController::remove(const QJsonObject &jsonObject)
{
    return impl->remove(jsonObject);
}





