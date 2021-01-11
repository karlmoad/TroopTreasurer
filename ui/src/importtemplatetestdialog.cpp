#include "ui/importtemplatetestdialog.h"
#include "ui_importtemplatetestdialog.h"
#include "objects/importspecificationruntime.h"
#include "objects/jsontablemodel.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QString>
#include <QList>
#include <QMap>

class ImportTemplateTestDialog::ImportTemplateTestDialogImpl
{
public:
    ImportTemplateTestDialogImpl(ImportTemplateTestDialog *dialog): _dialog(dialog), _spec(nullptr), _ui(new Ui::importtemplatetestdialog)
    {
        _ui->setupUi(_dialog);
        _output = new JsonTableModel(_dialog);
        _ui->tableResults->setModel(_output);
    }

    ~ImportTemplateTestDialogImpl()
    {
        delete _ui;
    }

    void setModel(QAbstractTableModel *model)
    {
        _input = model;
    }

    void setSpec(ImportSpecification *spec)
    {
        _spec = spec;
    }

    void load()
    {
        if(!_input || !_spec)
        {
            QMessageBox::critical(_dialog,"Error", "No input model and/or specification set, unable to process");
            return;
        }

        //load the input data model to the view
        _ui->tableData->setModel(_input);

        ImportSpecificationRuntime runtime(*_spec);
        runtime.setModel(_input);

        QList<QJsonObject> results;
        int rowCount = _input->rowCount();
        for(int i = 0; i<rowCount; i++)
        {
            results.append(runtime.process(i));
        }

        _output->setData(results);
    }

private:
    Ui::importtemplatetestdialog *_ui;
    QAbstractTableModel *_input;
    JsonTableModel *_output;
    ImportTemplateTestDialog *_dialog;
    ImportSpecification *_spec;
};

ImportTemplateTestDialog::ImportTemplateTestDialog(QAbstractTableModel *model, ImportSpecification *spec,
                                                   QWidget *parent): ImportTemplateTestDialog(parent)
{
    impl->setModel(model);
    impl->setSpec(spec);
}

ImportTemplateTestDialog::ImportTemplateTestDialog(QWidget *parent): QDialog(parent), impl(new ImportTemplateTestDialogImpl(this)){}

ImportTemplateTestDialog::~ImportTemplateTestDialog(){}

void ImportTemplateTestDialog::setModel(QAbstractTableModel *model)
{
    impl->setModel(model);
}

void ImportTemplateTestDialog::setSpecification(ImportSpecification *spec)
{
    impl->setSpec(spec);
}

void ImportTemplateTestDialog::load()
{
    impl->load();
}
