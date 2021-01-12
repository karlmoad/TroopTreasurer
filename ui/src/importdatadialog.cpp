#include "ui/importdatadialog.h"
#include "ui_importdatadialog.h"
#include "objects/importspecification.h"
#include "objects/importspecificationruntime.h"
#include "objects/schema.h"
#include "objects/utility.h"
#include "objects/csvmodel.h"
#include "ui/applicationconstants.h"
#include <QDebug>
#include <QMessageBox>

class ImportDataDialog::ImportDataDialogImpl
{
public:
    ImportDataDialogImpl(ImportDataDialog *dialog): _ui(new Ui::ImportDataDialog), _dialog(dialog)
    {
        _ui->setupUi(_dialog);
        _input = new CSVModel(_dialog);
        connect(_ui->btnCancel, &QPushButton::clicked, _dialog,  &ImportDataDialog::cancelHandler);
        connect(_ui->btnNext, &QPushButton::clicked, _dialog, &ImportDataDialog::nextHandler);
        connect(_ui->btnRun, &QPushButton::clicked, _dialog, &ImportDataDialog::executeHandler);
        connect(_ui->paneStack,&QStackedWidget::currentChanged, _dialog, &ImportDataDialog::activePaneChangedHandler);
        connect(_ui->cboTemplate, qOverload<int>(&QComboBox::currentIndexChanged), _dialog, &ImportDataDialog::templateSelected);
        initStartState();
    }

    ~ImportDataDialogImpl()
    {
        delete _ui;
    }

    void initTemplates()
    {
        _specs = ImportSpecificationController::Load(
                Utility::GetUserPreferencesFilePath(APP::ApplicationImportTemplatesFile));

        _ui->cboTemplate->clear();
        _ui->cboTemplate->addItem("(Select A File Type)",-1);
        try
        {
            for(int i=0; i<_specs.count(); i++)
            {
                _ui->cboTemplate->addItem(_specs[i].getName(), i);
            }
        }
        catch(ObjectError err)
        {
            if(err.errorCode() != (int)ObjectErrorCode::ERROR_NO_FILE)
            {
                QMessageBox::critical(_dialog,"File Error" ,"Error opening template definition file");
            }
        }

        _ui->cboTemplate->setCurrentIndex(0);
    }

    void initSchemas()
    {
        _schemas = Schema::load(":/resources/files/schema.json");
    }

    void initStartState()
    {
        _ui->paneStack->setCurrentIndex(0);
        _ui->btnNext->setEnabled(false);
        _ui->btnRun->setEnabled(false);
        _ui->chkDupCheck->setEnabled(false);
        _ui->chkTruncate->setEnabled(false);
        _ui->barProgress->setMinimum(0);
        _ui->barProgress->setMinimum(10);
        _ui->barProgress->setValue(0);
        _ui->lblProgress->setText("Preparing...");
    }

    void setInputData(const QString& filename)
    {
        QFile file(filename);
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(_dialog,"Input file error", "Unable to open file: " + file.errorString());
            return;
        }
        _input->load(&file, ',', true);
        file.close();

        initTemplates();
        initSchemas();
    }

    void activePaneChange(int index)
    {
        switch(index)
        {
            case 0:
            {
                _ui->btnNext->setEnabled(true);
                _ui->btnRun->setEnabled(false);
                break;
            }
            case 1:
            {
                _ui->btnNext->setEnabled(false);
                _ui->btnRun->setEnabled(true);
                break;
            }
            default:
            {
                _ui->btnNext->setEnabled(false);
                _ui->btnRun->setEnabled(false);
                break;
            }
        }
    }

    void next()
    {
        _ui->paneStack->setCurrentIndex(_ui->paneStack->currentIndex()+1);
    }

    void execute()
    {
        qDebug() << "Get ready for a surprise";
    }

    void evaluateTemplateAndSchema(int index)
    {
        if(index  > 0)
        {
            int idx = _ui->cboTemplate->currentData().toInt();
            if(idx > -1)
            {
                ImportSpecification spec = _specs.at(idx);
                if(_schemas.contains(spec.getTarget()))
                {
                    Schema schema = _schemas[spec.getTarget()];
                    if(schema.isImportSupported())
                    {
                        QMap<QString, QJsonObject> schemaFields = schema.getFields();
                        QList<QString> specFields = spec.getFields();
                        bool pass = true;

                        //evaluate schema non-nullable fields to spec
                        for (QString field : schemaFields.keys())
                        {
                            bool nullable = schemaFields[field]["nullable"].toBool(true);
                            if (!nullable && !specFields.contains(field))
                            {
                                pass = false;
                                break;
                            }
                        }

                        if (!pass)
                        {
                            QMessageBox::critical(_dialog, "Error", "Import specification does not match schema");
                        }
                        else
                        {
                            _ui->btnNext->setEnabled(true);
                            _ui->chkTruncate->setEnabled(schema.isTruncateSupported());
                            _ui->chkDupCheck->setEnabled(schema.isDupCheckSupported());
                        }
                    }
                }
            }
        }
    }

private:
    Ui::ImportDataDialog *_ui;
    ImportDataDialog *_dialog;
    CSVModel *_input;
    QList<ImportSpecification> _specs;
    QMap<QString, Schema> _schemas;
};

ImportDataDialog::ImportDataDialog(const QString& filename, QWidget *parent) :
    QDialog(parent),
    impl(new ImportDataDialogImpl(this))
{
    impl->setInputData(filename);
}

ImportDataDialog::~ImportDataDialog()
{}

void ImportDataDialog::activePaneChangedHandler(int index)
{
    impl->activePaneChange(index);
}

void ImportDataDialog::nextHandler()
{
    impl->next();
}

void ImportDataDialog::executeHandler()
{
    impl->execute();
}

void ImportDataDialog::cancelHandler()
{
    this->close();
}

void ImportDataDialog::templateSelected(int index)
{
    impl->evaluateTemplateAndSchema(index);
}
