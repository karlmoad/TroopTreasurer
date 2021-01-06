#ifndef NEWIMPORTTEMPLATEDIALOG_H
#define NEWIMPORTTEMPLATEDIALOG_H

#include <QDialog>
#include <memory>

class NewImportTemplateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewImportTemplateDialog(QString& name, QString& target, QWidget *parent = nullptr);
    ~NewImportTemplateDialog();

    Q_SLOT void okHandler();
    Q_SLOT void cancelHandler();

private:
    class NewImportTemplateDialogImpl;
    std::shared_ptr<NewImportTemplateDialogImpl> impl;

};

#endif // NEWIMPORTTEMPLATEDIALOG_H
