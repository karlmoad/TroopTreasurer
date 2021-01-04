#include "objects/expressionfactory.h"

const QMap<QString,int> ExpressionFactory::_funcMap = QMap<QString,int>{
        {"UUID",0},
        {"BATCHID",1},
        {"HASH256",2},
        {"REMCHARS",3},
        {"CONCAT",4},
        {"TRIM",5}};


Expression* ExpressionFactory::FindExpression(const QString &name, int argCount)
{
    if(ExpressionFactory::_funcMap.contains(name.toUpper().trimmed()))
    {
        switch (ExpressionFactory::_funcMap[name.toUpper().trimmed()])
        {
            case 0:
                return new UUID();
            case 1:
                return new BatchId();
            case 2:
                return new Hash256();
            case 3:
                return new RemSpecial();
            case 4:
                return new Concat(argCount);
            case 5:
                return new Trim();
            default:
                return nullptr;
        }
    }
    return nullptr;
}

Expression *ExpressionFactory::CreateRecordValueExpression(int ref)
{
    return new RecordValue(ref);
}

Expression *ExpressionFactory::CreateValueOfExpression(QVariant value)
{
    return new ValueOf(value);
}

Expression *ExpressionFactory::CompileExpression(const QString &buffer, const QList<QString>& sourceColumns)
{
    enum State{Normal, Scope};

    if(buffer.size() > 1)
    {
        int pos = 0;
        QChar c = buffer.at(pos);
        if(c == '$')
        {
            int s = buffer.indexOf('(',pos);
            int e = buffer.lastIndexOf(')');

            if(s == -1 || e == -1)
            {
                ObjectError err("Unable to parse expression, invalid parentheses match", static_cast<int>(ObjectErrorCode::EXPRESSION_PARSING));
                err.raise();
            }

            QString func = buffer.mid(1, s-1);
            QString params = buffer.mid(s+1, e-s-1).trimmed();
            QQueue<QString> paramBuffer;

            //qDebug() << "Parsed Func: " << func << " Args: " << params;

            //Determine if there are param args
            int argz = 0;
            if(params.size() > 0)
            {
                int sep = 0;
                QList<int> seps;
                int scopeDepth = 0;
                for (int i = 0; i < params.size(); i++)
                {
                    QChar t = params.at(i);
                    if (scopeDepth==0 && t == ',')
                    {
                        seps.append(i);
                        sep++;
                    }
                    if (t == '(') scopeDepth++;
                    if (t == ')') scopeDepth--;
                }

                if (sep == 0)
                {
                    paramBuffer.enqueue(params);
                }
                else
                {

                    //enqueue each of the params for next cycle
                    for (int x = 0; x < seps.size(); x++)
                    {
                        if(x==0)
                            paramBuffer.enqueue(params.mid(0,seps[x]));
                        else
                        {
                            paramBuffer.enqueue(params.mid(seps[x-1]+1, seps[x] - (seps[x-1]+1)));
                        }
                    }
                    //add on last param
                    paramBuffer.append(params.mid(seps[seps.size()-1]+1));
                }
                argz = (sep>0?sep+1:1);

                //qDebug() << "Function: " << func << " Arg Count: " << argz << " Args: " << paramBuffer << " Raw Params: " << params;
            }

            //map the function
            Expression *funcRef = ExpressionFactory::FindExpression(func, argz);
            while(!paramBuffer.isEmpty())
            {
                Expression* ref = ExpressionFactory::CompileExpression(paramBuffer.dequeue(), sourceColumns);
                Argument *arg = new Argument(ref);
                funcRef->addArgument(arg);
            }
            return funcRef;
        }
        else if(c=='@')
        {
            QString field = buffer.mid(1);
            int idx = -1;
            if(sourceColumns.contains(field))
            {
                idx = sourceColumns.indexOf(field,0);
            }
            return ExpressionFactory::CreateRecordValueExpression(idx);
        }
        else if(c=='\'')
        {
            int e = buffer.lastIndexOf('\'');
            if(e == -1)
            {
                ObjectError err("Unable to parse and compile expression as defined", static_cast<int>(ObjectErrorCode::EXPRESSION_PARSING));
                err.raise();
            }
            QString value = buffer.mid(pos+1, e-1-pos);
            return ExpressionFactory::CreateValueOfExpression(value);
        }
    }

    return nullptr;
}