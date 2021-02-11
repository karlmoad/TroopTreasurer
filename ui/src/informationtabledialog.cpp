#include "ui/informationtabledialog.h"
#include "ui_informationtabledialog.h"
#include "objects/jsontablemodel.h"

class InformationTableDialog::InformationTableDialogImpl
{
public:
    InformationTableDialogImpl(InformationTableDialog *dialog): _dialog(dialog), _ui(new Ui::InformationTableDialog)
    {
        _ui->setupUi(_dialog);
        _model = new JsonTableModel(_dialog);
        _ui->tableInformation->setModel(_model);
    }

    ~InformationTableDialogImpl()
    {
        delete _ui;
    }

    void setHeaders(const QList<QJsonObject>& headers)
    {
        _model->setHeaders(headers);
    }

    void setData(const QList<QJsonObject> &data)
    {
        _model->setData(data);
    }

    void setTitle(const QString& title)
    {
        _dialog->setWindowTitle(title);
    }

private:
    InformationTableDialog *_dialog;
    Ui::InformationTableDialog *_ui;
    JsonTableModel *_model;
};

InformationTableDialog::InformationTableDialog(QWidget *parent) :
    QDialog(parent),
    impl(new InformationTableDialogImpl(this))
{

}

InformationTableDialog::~InformationTableDialog(){}

void InformationTableDialog::setHeaderMap(const QList<QJsonObject>& headers)
{
    impl->setHeaders(headers);
}

void InformationTableDialog::setData(const QList<QJsonObject> &data)
{
    impl->setData(data);
}

void InformationTableDialog::setDialogTitle(const QString &title)
{
    impl->setTitle(title);
}
