#include "objects/importspecificationruntime.h"

class ImportSpecificationRuntime::ImportSpecificationRuntimeImpl
{
public:
    explicit ImportSpecificationRuntimeImpl(const ImportSpecification &spec): _spec(spec)
    {}

    ~ImportSpecificationRuntimeImpl()
    {
        qDeleteAll(_map);
    }

    bool compile()
    {
        bool compiled = false;
        try
        {
            processSpec(_spec);
            compiled = true;
        }
        catch(ObjectError err)
        {
            compiled= false;
            err.raise();
        }

        return compiled;
    }

    void setModel(QAbstractTableModel *model)
    {
        _model = model;
        _columnCount = _model->columnCount();
        _rowCount = _model->rowCount();
    }

    QJsonObject process(int row)
    {
        QJsonObject out;
        QList<QString> record;

        //get record from model
        for(int i=0;i<_columnCount;i++)
        {
            QModelIndex idx = _model->index(row, i);
            record.append(_model->data(idx).toString());
        }

        //process against expressions to produce output
        for(QString key : _map.keys())
        {
            out[key] = _map[key]->execute(record).toJsonValue();
        }
        return out;
    }

private:

    ImportSpecification _spec;
    QList<QString> _sourceColumns;
    QMap<QString,Expression*> _map;
    bool _errorState;
    QString _errorStr;
    QAbstractTableModel *_model;
    int _columnCount;
    int _rowCount;

    const QRegularExpression functionRe = QRegularExpression("(?<=\\$)([A-Za-z0-9]*)(?=\\(.*\\))");
    const QRegularExpression fieldRe = QRegularExpression("(?<=\\@\\()([A-Za-z0-9\\s]*)(?=\\))");

    void processSpec(const ImportSpecification &spec)
    {
        QList<QString> fields = spec.getFields();
        for(int i=0; i<fields.count(); i++)
        {
            initFieldExpression(fields[i], spec.getFieldExpression(fields[i]));
        }
    }

    void initFieldExpression(const QString &key, const QString &op)
    {
        try
        {
            Expression *e = ExpressionFactory::CompileExpression(op);
            if (e != nullptr)
            {
                this->_map[key] = e;
            }
        }
        catch(ObjectError ex)
        {
            ObjectError err(QString("Error compiling expression for key: [%1], %2").arg(key,ex.what()),ex.errorCode());
            err.raise();
        }
    }
};

ImportSpecificationRuntime::ImportSpecificationRuntime(const ImportSpecification& spec): impl(new ImportSpecificationRuntimeImpl(spec))
{}

ImportSpecificationRuntime::~ImportSpecificationRuntime()
{}

bool ImportSpecificationRuntime::compile()
{
    return impl->compile();
}

void ImportSpecificationRuntime::setModel(QAbstractTableModel *model)
{
    impl->setModel(model);
}

QJsonObject ImportSpecificationRuntime::process(int row)
{
    return impl->process(row);
}
