#ifndef IMPORTDATADIALOG_H
#define IMPORTDATADIALOG_H

#include <QDialog>
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

private:
    class ImportDataDialogImpl;
    std::shared_ptr<ImportDataDialogImpl> impl;
};

#endif // IMPORTDATADIALOG_H
