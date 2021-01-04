//
// Created by Karl Moad on 1/4/21.
//

#include "objects/csvmodel.h"

struct CSVModel::CSVModelImpl
{
    int _colCount;
    QVector<QString> _headers;
    QVector<QVector<QString>> _data;

    void loadSource(QIODevice *file, bool withHeader,QChar sep)
    {
        if(withHeader)
        {
            if(file->atEnd())
            {
                ObjectError err("Unable to open and read file, end of file reached", static_cast<int>(ObjectErrorCode::ERROR_READ_FILE));
                err.raise();
                return;
            }

            QString firstLine = file->readLine();
            firstLine.remove('\n');
            firstLine.remove('\r');

            _headers = parseRecord(firstLine, sep);
            _colCount = _headers.size();
        }
        int ln = 0;
        while (!file->atEnd())
        {
            QString l = file->readLine();
            l.remove('\n');
            l.remove('\r');

            QVector<QString> fields = parseRecord(l, sep);
            if(_colCount < fields.size())
            {
                _colCount = fields.size();
            }

            _data.append(fields);

            ln++;
        }
    }

    QVector<QString> parseRecord(const QString& raw, QChar sep)
    {
        enum State {Normal, Quote} state = Normal;
        QVector<QString> fields;
        QString value;

        for (int i = 0; i < raw.size(); i++)
        {
            const QChar current = raw.at(i);

            // Normal state
            if (state == Normal)
            {
                // Comma
                if (current == sep)
                {
                    // Save field
                    fields.append(value.trimmed());
                    value.clear();
                }

                    // Double-quote
                else if (current == '"')
                {
                    state = Quote;
                    value += current;
                }

                    // Other character
                else
                    value += current;
            }

                // In-quote state
            else if (state == Quote)
            {
                // Another double-quote
                if (current == '"')
                {
                    if (i < raw.size())
                    {
                        // A double double-quote?
                        if (i+1 < raw.size() && raw.at(i+1) == '"')
                        {
                            value += '"';

                            // Skip a second quote character in a row
                            i++;
                        }
                        else
                        {
                            state = Normal;
                            value += '"';
                        }
                    }
                }
                else
                    value += current;
            }
        }

        fields.append(value.trimmed());

        // Quotes are left in until here; so when fields are trimmed, only whitespace outside of
        // quotes is removed.  The quotes are removed here.
        for (int i=0; i<fields.size(); ++i)
            if (fields[i].length()>=1 && fields[i].left(1)=='"')
            {
                fields[i]=fields[i].mid(1);
                if (fields[i].length()>=1 && fields[i].right(1)=='"')
                    fields[i]=fields[i].left(fields[i].length()-1);
            }

        return fields;
    }
};

CSVModel::CSVModel(QObject *parent) : QAbstractTableModel(parent), impl(new CSVModelImpl())
{
    impl->_colCount=0;
}

CSVModel::~CSVModel()
{
    delete impl;
}

int CSVModel::rowCount(const QModelIndex &parent) const
{
    return impl->_data.size();
}

QVariant CSVModel::data(const QModelIndex &index, int role) const
{
    if( role != Qt::DisplayRole )
    {
        return QVariant();
    }

    if(index.row() < impl->_data.size() && index.column() < impl->_data.at(index.row()).size())
    {
        return impl->_data.at(index.row()).at(index.column());
    }
    return QVariant();
}

QVariant CSVModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( role != Qt::DisplayRole )
    {
        return QVariant();
    }

    switch(orientation)
    {
        case Qt::Horizontal:
        {
            if (impl->_colCount > 0 && section < impl->_colCount)
            {
                if (impl->_headers.size()-1 >= section)
                {
                    return impl->_headers.at(section);
                } else
                {
                    return QString::number(section);
                }
            }
        }
        case Qt::Vertical:
        {
            return section + 1;
        }
        default:
            return QVariant();
    }
}

Qt::ItemFlags CSVModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;  // only applicable as table and editing is not allowed only display based model
}

int CSVModel::columnCount(const QModelIndex &parent) const
{
    return impl->_colCount;
}

QList<QString> CSVModel::getColumnHeaders()
{
    return impl->_headers.toList();
}

QList<QString> CSVModel::getRow(int i)
{
    if(i < impl->_data.size())
    {
        return impl->_data.at(i).toList();
    }
    return QList<QString>();
}

void CSVModel::load(QFile *file, QChar sep, bool includesHeader)
{
    if(file == nullptr)
    {
        ObjectError err("Unable to open and read file, null file", static_cast<int>(ObjectErrorCode::ERROR_READ_FILE));
        err.raise();
    }
    else
    {
        beginResetModel();
        impl->loadSource(file,includesHeader,sep);
        endResetModel();
    }
}
