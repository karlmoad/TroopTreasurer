#include "objects/importspecificationruntime.h"

class ImportSpecificationRuntime::ImportSpecificationRuntimeImpl
{
public:
    explicit ImportSpecificationRuntimeImpl(const ImportSpecification &spec,
                                            const QList<QString> &sourceColumns)
    {
        _sourceColumns = sourceColumns;
        processSpec(spec);
    }

    ~ImportSpecificationRuntimeImpl()
    {
        qDeleteAll(_map);
    }

    QMap<QString,QVariant> process(const QList<QString> &record)
    {
        QMap<QString, QVariant> out;

        for(QString key : _map.keys())
        {
            out[key] = _map[key]->execute(record);
        }
        return out;
    }

private:

    QList<QString> _sourceColumns;
    QMap<QString,Expression*> _map;
    bool _errorState;
    QString _errorStr;

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
            Expression *e = ExpressionFactory::CompileExpression(op, _sourceColumns);
            if (e != nullptr)
            {
                this->_map[key] = e;
            }
        }
        catch(ObjectError ex)
        {
            qDebug() << "Error Compiling Expression for Key: " << key;
            throw ex;
        }
    }
};

ImportSpecificationRuntime::ImportSpecificationRuntime(const ImportSpecification &spec,
                                                       const QList<QString> &sourceColumns): impl(new ImportSpecificationRuntimeImpl(spec,sourceColumns))
{}

ImportSpecificationRuntime::~ImportSpecificationRuntime()
{}

QMap<QString, QVariant> ImportSpecificationRuntime::process(const QList<QString> &record)
{
    return impl->process(record);
}
