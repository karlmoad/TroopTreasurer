#ifndef IMPORTDATADIALOG_H
#define IMPORTDATADIALOG_H

#include <QDialog>
#include <QPoint>
#include <memory>

class ImportDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportDataDialog(const QString& filename, QWidget *parent = nullptr);
    ~ImportDataDialog();

public slots:
    void activePaneChangedHandler(int index);
    void nextHandler();
    void executeHandler();
    void cancelHandler();
    void templateSelected(int index);
    void progressNotificationHandler(int index, QJsonObject data);
    void completionNotificationHandler(int processed, int successful, int duplicates, int errors);
    void showDetailResults();

private:
    class ImportDataDialogImpl;
    std::shared_ptr<ImportDataDialogImpl> impl;
};

#endif // IMPORTDATADIALOG_H
