#include "ui/importdatadialog.h"
#include "ui_importdatadialog.h"
#include "objects/importspecification.h"
#include "objects/dataimportcontroller.h"
#include "objects/schema.h"
#include "objects/utility.h"
#include "objects/csvmodel.h"
#include "objects/settingsmanager.h"
#include "ui/applicationconstants.h"
#include "ui/applicationsettings.h"
#include <QDebug>
#include <QMessageBox>
#include <QThread>

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

    void progressNotification(int index, QJsonObject data)
    {
        if(data.contains("stage"))
        {
            if(data["stage"].toString().compare("initialization")==0 || data["stage"].toString().compare("truncate")==0)
            {
                QMessageBox::critical(_dialog, "Error", data["message"].toString());
            }
            else
            {
                if(data.contains("status") && !data["status"].toBool(false))
                {
                    _messages.append(data);
                }
            }
        }
        _ui->barProgress->setValue(index+1);
    }

    void completionNotification(int processed, int successful, int duplicates, int errors)
    {
        _ui->barProgress->setValue(_ui->barProgress->maximum());
        _ui->listReport->addItem(QString("Processed: %1").arg(QString::number(processed)));
        _ui->listReport->addItem(QString("Successful: %1").arg(QString::number(successful)));
        _ui->listReport->addItem(QString("Duplicates: %1").arg(QString::number(duplicates)));
        _ui->listReport->addItem(QString("Errors: %1").arg(QString::number(errors)));
        _ui->lblProgress->setText("Completed");
    }

    void initStartState()
    {
        _ui->paneStack->setCurrentIndex(0);
        _ui->btnNext->setEnabled(false);
        _ui->btnRun->setEnabled(false);
        _ui->chkDupCheck->setEnabled(false);
        _ui->chkTruncate->setEnabled(false);
        _ui->barProgress->setMinimum(0);
        _ui->barProgress->setMaximum(10);
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

        _ui->barProgress->setMaximum(_input->rowCount(QModelIndex()));

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
        _ui->btnRun->setEnabled(false);
        //clear out old processing messages
        _messages.clear();

        ImportSpecification spec = _specs.at(_currentSpec);
        Schema schema = _schemas[_currentSchema];

        bool trunc = _ui->chkTruncate->checkState() == Qt::Checked;
        bool dups = _ui->chkDupCheck->checkState() == Qt::Checked;

        if(trunc)
        {
            QMessageBox::StandardButton resp = QMessageBox::question(_dialog, "Confirm",
                                                                     "Are you sure you wish to truncate existing data?");
            if(resp == QMessageBox::No)
            {
                trunc = false;
            }

        }

        QJsonObject dbSettings = SettingsManager::getInstance()->getSettingsSegment(ApplicationSettingsUtility::ApplicationSettingTypeToString(ApplicationSettingsType::DATABASE));

        try
        {
            auto controller = std::shared_ptr<DataImportController>(DataImportController::Builder()
                    .setSchema(schema)
                    .setDatabaseSettings(dbSettings)
                    .setSpecification(&spec)
                    .setModel(_input)
                    .setOptionTruncate(trunc)
                    .setOptionDuplicateCheck(dups)
                    .build());

            connect(controller.get(), &DataImportController::notifyProgress, _dialog , &ImportDataDialog::progressNotificationHandler);
            connect(controller.get(), &DataImportController::notifyCompletion, _dialog, &ImportDataDialog::completionNotificationHandler);
            connect(controller.get(), &DataImportController::finished, [this](){
                _ui->btnCancel->setEnabled(true);
            });

            //setup thread worker
            QThread *thread = new QThread();
            controller->moveToThread(thread);
            connect(thread, &QThread::started, controller.get(), &DataImportController::start);
            connect(controller.get(), &DataImportController::finished, thread, &QThread::quit);
            //connect(controller.get(), &DataImportController::finished, controller.get(), &DataImportController::deleteLater);
            connect(thread, &QThread::finished, thread, &QThread::deleteLater);
            thread->start();
        }
        catch(ObjectError err)
        {
            QMessageBox::critical(_dialog, "Error", "Error unable to process file: " + QString(err.what()));
        }
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
                            _currentSpec = idx;
                            _currentSchema = schema.getName();
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
    QList<QJsonObject> _messages;
    int _currentSpec;
    QString _currentSchema;
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

void ImportDataDialog::progressNotificationHandler(int index, QJsonObject data)
{
    impl->progressNotification(index,data);
}

void ImportDataDialog::completionNotificationHandler(int processed, int successful, int duplicates, int errors)
{
    impl->completionNotification(processed, successful, duplicates, errors);
}
