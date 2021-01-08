#ifndef NEWIMPORTTEMPLATEDIALOG_H
#define NEWIMPORTTEMPLATEDIALOG_H

#include <QDialog>
#include <memory>
#include "objects/schema.h"

class NewImportTemplateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewImportTemplateDialog(QString *name, Schema *target, QWidget *parent = nullptr);
    ~NewImportTemplateDialog();

    Q_SLOT void okHandler();
    Q_SLOT void cancelHandler();

private:
    class NewImportTemplateDialogImpl;
    std::shared_ptr<NewImportTemplateDialogImpl> impl;

};

#endif // NEWIMPORTTEMPLATEDIALOG_H
