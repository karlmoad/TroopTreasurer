#ifndef INFORMATIONTABLEDIALOG_H
#define INFORMATIONTABLEDIALOG_H

#include <QDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QMap>
#include <QList>
#include <QString>
#include <memory>

class InformationTableDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InformationTableDialog(QWidget *parent = nullptr);
    ~InformationTableDialog();

    void setHeaderMap(const QMap<QString,QString> &headers);
    void setData(const QList<QJsonObject> &data);
    void setDialogTitle(const QString& title);

private:
    class InformationTableDialogImpl;
    std::shared_ptr<InformationTableDialogImpl> impl;
};

#endif // INFORMATIONTABLEDIALOG_H
