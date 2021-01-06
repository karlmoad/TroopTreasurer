#include "ui/newimporttemplatedialog.h"
#include "ui_newimporttemplatedialog.h"
#include "objects/schema.h"
#include <QMap>

class NewImportTemplateDialog::NewImportTemplateDialogImpl
{
public:
    NewImportTemplateDialogImpl(QString& name, QString& target, NewImportTemplateDialog *form)
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
            _name = ui->txtName->text();
            _target = ui->cboTarget->currentData().toString();
            return true;
        }
        else
            return false;
    }

private:
    Ui::NewImportTemplateDialog *ui;
    QString _name;
    QString _target;

    void init()
    {
        ui->cboTarget->addItem("(Select a Target Schema)","");
        QMap<QString, Schema> schemas = Schema::load(":/resources/files/schema.json");
        for(QString key: schemas.keys())
        {
            ui->cboTarget->addItem(schemas[key].getDisplay(), key);
        }
    }
};

NewImportTemplateDialog::NewImportTemplateDialog(QString& name, QString& target, QWidget *parent) :
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
