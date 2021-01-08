#include "ui/newimporttemplatedialog.h"
#include "ui_newimporttemplatedialog.h"
#include "objects/schema.h"
#include <QMap>

class NewImportTemplateDialog::NewImportTemplateDialogImpl
{
public:
    NewImportTemplateDialogImpl(QString *name, Schema *target, NewImportTemplateDialog *form)
    {
        ui = new Ui::NewImportTemplateDialog();
        ui->setupUi(form);
        _name = name;
        _target = target;
        init();
        connect(ui->btnCancel, &QPushButton::clicked, form, &NewImportTemplateDialog::cancelHandler);
        connect(ui->btnOk, &QPushButton::clicked, form, &NewImportTemplateDialog::okHandler);
    }

    ~NewImportTemplateDialogImpl()
    {
        delete ui;
    }

    bool sync()
    {
        if(ui->cboTarget->currentIndex() > 0 && ui->txtName->text().trimmed().length() > 0)
        {
            *_name = ui->txtName->text();
            QString key = ui->cboTarget->currentData().toString();
            if(_schemas.contains(key))
            {
                *_target = _schemas[key];
            }
            return true;
        }
        else
            return false;
    }

private:
    Ui::NewImportTemplateDialog *ui;
    QString *_name;
    Schema *_target;
    QMap<QString, Schema> _schemas;

    void init()
    {
        ui->cboTarget->addItem("(Select a Target Schema)","");
        _schemas = Schema::load(":/resources/files/schema.json");
        for(QString key: _schemas.keys())
        {
            ui->cboTarget->addItem(_schemas[key].getDisplay(), key);
        }
    }
};

NewImportTemplateDialog::NewImportTemplateDialog(QString *name, Schema *target, QWidget *parent) :
    QDialog(parent),
    impl(new NewImportTemplateDialogImpl(name, target, this))
{}

NewImportTemplateDialog::~NewImportTemplateDialog()
{}

void NewImportTemplateDialog::okHandler()
{
    if(impl->sync())
    {
        this->done(QDialog::Accepted);
        this->close();
    }
}

void NewImportTemplateDialog::cancelHandler()
{
    this->done(QDialog::Rejected);
    this->close();
}
