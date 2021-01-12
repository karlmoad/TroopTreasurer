#include "objects/dataimportcontroller.h"
#include "objects/settingsmanager.h"
#include "objects/datatype.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlResult>
#include <QCoreApplication>

class DataImportController::DataImportControllerImpl
{
public:
    enum class StatementType{DUPCHECK,PERIST,QUERY,OTHER};
    DataImportControllerImpl(DataImportController *c): _controller(c), _runtime(nullptr)
    {}

    ~DataImportControllerImpl()
    {
        if(_runtime) delete _runtime;
    }

    void setSpecification(ImportSpecification *spec)
    {
        _spec = spec;
    }

    void setInput(QAbstractTableModel *model)
    {
        _input = model;
    }

    void setSchema(const Schema& schema)
    {
        _schema = schema;
    }

    void setDuplicateCheckOption(bool option)
    {
        _dup = option;
    }

    void setTruncateOption(bool option)
    {
        _trunc = option;
    }

    void setDatabaseSettings(const QJsonObject& settings)
    {
        _dbSettings = settings;
    }


    void prep()
    {
        _runtime = new ImportSpecificationRuntime(*_spec);
        _runtime->compile();
        _runtime->setModel(_input);
    }

    void run()
    {
        int dups = 0;
        int errors = 0;
        int processed = 0;
        int success = 0;

        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL","IMPORTTHREAD");
        db.setDatabaseName(_dbSettings["schema"].toString(""));
        db.setHostName(_dbSettings["host"].toString(""));
        db.setPort(_dbSettings["port"].toInt(3306));
        db.setUserName(_dbSettings["user"].toString(""));
        db.setPassword(_dbSettings["password"].toString(""));
        QString opts;
        opts.append(QString("SSL_CA=%1;").arg(_dbSettings["ca_crt"].toString("")));
        opts.append(QString("SSL_CERT=%1;").arg(_dbSettings["svr_crt"].toString("")));
        opts.append(QString("SSL_KEY=%1;").arg(_dbSettings["svr_key"].toString("")));
        db.setConnectOptions(opts);

        if(!db.open())
        {
            QJsonObject msg;
            msg["stage"] = "initialization";
            msg["message"] = "database connection not established: " + db.lastError().text();
            msg["status"] = false;
            emit _controller->notifyProgress(-1,msg);
            emit _controller->notifyCompletion(0,0,0,0);
            emit _controller->finished();
            return;
        }

        int rowCount = _input->rowCount();
        if(rowCount > 0) // if row count is 0 not much value doing anything else
        {
            if (!db.isValid())
            {
                ObjectError err("Unable to acquire database connection",
                                static_cast<int>(ObjectErrorCode::DATABASE_ERROR));
                err.raise();
                return;
            }

            if (_trunc)
            {
                QJsonObject msg;
                if(!truncateData(db,&msg))
                {
                    msg["stage"] = "Truncate";
                    msg["status"] = false;
                    emit _controller->notifyProgress(-1,msg);
                    emit _controller->notifyCompletion(0,0,0,0);
                    return;
                }
            }

            for (int i = 0; i < rowCount; i++)
            {
                QJsonObject msg;
                msg["index"] = i+1;

                QJsonObject record= _runtime->process(i);
                processed += 1;
                if(_dup)
                {
                    if(checkExistingRecords(record,db, &msg))
                    {
                        // a record may already exist inform
                        msg["stage"] = "duplicate check";
                        msg["duplicate"] = true;
                        msg["status"] = false;
                        msg["record"] = i;
                        emit _controller->notifyProgress(i, msg);
                        dups += 1;
                        continue; //go onto next record
                    }
                }

                if (!persistRecord(record, db, &msg))
                {
                    msg["stage"] = "persist";
                    msg["error"] = true;
                    msg["status"] = false;
                    msg["record"] = i;
                    errors += 1;
                    emit _controller->notifyProgress(i, msg);
                    continue;  //go oto next
                }
                else
                {
                    success += 1;
                    msg["stage"] = "persist";
                    msg["status"] = true;
                    msg["record"] = i;
                    emit _controller->notifyProgress(i, msg);
                }
            }
            emit _controller->notifyCompletion(processed,success,dups,errors);
        }
        db.close();
        QSqlDatabase::removeDatabase("IMPORTTHREAD");
        emit _controller->finished();
    }

private:
    bool truncateData(QSqlDatabase &db, QJsonObject *error)
    {
        bool out = false;
        if(_trunc && _schema.isTruncateSupported())
        {
            QString stmt = QString("DELETE FROM %1").arg(_schema.getTable());
            QSqlQuery query(db);
            out = query.exec(stmt);
            if(!out)
            {
                (*error)["message"] = query.lastError().text();
            }
        }
        return out;
    }

    bool checkExistingRecords(const QJsonObject &record, QSqlDatabase &db, QJsonObject *error)
    {
        QSqlQuery query(db);
        QString statement = renderStatement(StatementType::DUPCHECK, record);
        (*error)["statement"] = statement;
        if(!query.exec(statement))
        {
            (*error)["message"] = query.lastError().text();
            return true;
        }

        int i = 0;

        if(query.first())
        {
            i = query.value(0).toInt();
        }

        if(i <= 0)
        {
            return false;
        }
        // err on side of caution return true so that later overlay/dup records inserted ...dont cause issues
        return true;
    }

    bool persistRecord(const QJsonObject &record, QSqlDatabase &db, QJsonObject *error)
    {
        QSqlQuery query(db);
        QString statement = renderStatement(StatementType::PERIST, record);
        if(!query.exec(statement))
        {
            if(!query.exec())
            {
                (*error)["statement"] = query.executedQuery();
                (*error)["message"] = query.lastError().text();
                return false;
            }
        }
        return true;
    }

    QList<QJsonObject> getMatchKeyFields()
    {
        QList<QJsonObject> out;
        QMap<QString, QJsonObject> fields = _schema.getFields();

            for(QString field : fields.keys())
            {
                QJsonObject f= fields[field];
                if(f.contains("match_key") && f["match_key"].toBool(false))
                {
                    out.append(f);
                }
            }

        return out;
    }

    QString renderStatement(StatementType type, const QJsonObject &record)
    {
        QString stmt;

        switch(type)
        {
            case StatementType::DUPCHECK:
            {
                QList<QJsonObject> keyFields = getMatchKeyFields();
                //build check dup statement
                if(keyFields.size() > 0)
                {
                    stmt = QString("SELECT COUNT(*) FROM %1 WHERE ").arg(_schema.getTable());
                    for (int i = 0; i < keyFields.size(); i++)
                    {
                        QJsonObject keyField = keyFields.at(i);
                        DataType fieldType = DataType(keyField["dt"].toString());
                        if (i > 0)
                        {
                            stmt.append(" AND ");
                        }
                        QVariant fieldValue = record[keyField["name"].toString()];
                        QString criteria = QString("%1=%2").arg(keyField["name"].toString(), (fieldType.isQuoted() ? QString("'%1'").arg(fieldValue.toString()): fieldValue.toString()));
                        stmt.append(criteria);
                    }
                }
                break;
            }
            case StatementType::PERIST:
            {
                QQueue<QJsonObject> prep;
                QMap<QString, QJsonObject> fields = _schema.getFields();
                for(QString field: fields.keys())
                {
                    prep.enqueue(fields[field]);
                }

                QString f, v;
                int x = 0;
                while(!prep.isEmpty())
                {
                    QJsonObject field = prep.dequeue();
                    DataType fieldType = DataType(field["dt"].toString());
                    if(x > 0)
                    {
                        f.append(",");
                        v.append(",");
                    }
                    QVariant fieldValue = record[field["name"].toString()];
                    f.append(field["name"].toString());
                    v.append((fieldType.isQuoted() ? QString("'%1'").arg(fieldValue.toString()): fieldValue.toString()));
                    x++;
                }
                stmt = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(_schema.getTable(),f,v);
                break;
            }
            default:
                break;
        }

        return stmt;
    }

private:
    ImportSpecificationRuntime *_runtime;
    DataImportController *_controller;
    ImportSpecification *_spec;
    Schema _schema;
    QAbstractTableModel *_input;
    QJsonObject _dbSettings;
    bool _dup;
    bool _trunc;
};

DataImportController::~DataImportController(){}

void DataImportController::start()
{
    impl->run();
}

DataImportController::DataImportController(QObject *parent) : QObject(parent), impl(new DataImportControllerImpl(this)) {}

// Builder
DataImportController::Builder::Builder(QObject *parent): _instance(new DataImportController(parent)){}

DataImportController::Builder::~Builder(){}

DataImportController::Builder &DataImportController::Builder::setModel(QAbstractTableModel *input)
{
    _instance->impl->setInput(input);
    return *this;
}

DataImportController::Builder &DataImportController::Builder::setSpecification(ImportSpecification *spec)
{
    _instance->impl->setSpecification(spec);
    return *this;
}

DataImportController::Builder &DataImportController::Builder::setSchema(const Schema &schema)
{
    _instance->impl->setSchema(schema);
    return *this;
}

DataImportController::Builder &DataImportController::Builder::setOptionDuplicateCheck(bool option)
{
    _instance->impl->setDuplicateCheckOption(option);
    return *this;
}

DataImportController::Builder &DataImportController::Builder::setOptionTruncate(bool option)
{
    _instance->impl->setTruncateOption(option);
    return *this;
}

DataImportController *DataImportController::Builder::build()
{
    _instance->impl->prep();
    return _instance;
}

DataImportController::Builder &DataImportController::Builder::setDatabaseSettings(const QJsonObject &settings)
{
    _instance->impl->setDatabaseSettings(settings);
    return *this;
}
