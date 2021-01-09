#include <QString>
#include <QStandardPaths>
#include <QFileDialog>
#include "objects/importspecification.h"
#include "objects/importspecificationruntime.h"
#include "objects/utility.h"
#include "objects/templatemappingmodel.h"
#include "objects/csvmodel.h"
#include "objects/objecterror.h"
#include "objects/schema.h"
#include "ui/importtemplatepanel.h"
#include "ui_importtemplatepanel.h"
#include "ui/applicationconstants.h"
#include "ui/newimporttemplatedialog.h"
#include "ui/panelfactory.h"
#include "ui/panelactions.h"
#include <QDebug>

class ImportTemplatePanel::ImportTemplatePanelImpl
{
public:
    ImportTemplatePanelImpl(ImportTemplatePanel *panel): ui(new Ui::ImportTemplatePanel)
    {
        this->_panel = panel;
        ui->setupUi(_panel);

        _model = new TemplateMappingModel(_panel);
        _delegate = new TemplateMappingDelegate(_panel);
        _sample = new CSVModel(_panel);
        ui->tableTemplate->setModel(_model);
        ui->tableTemplate->setItemDelegate(_delegate);
        ui->tableData->setModel(_sample);

        //set initial state
        ItemState initialState;
        initialState.setDeleteEnabled(false);
        initialState.setAddEnabled(true);
        initialState.setEditEnabled(true);
        initialState.setSaveEnabled(true);
        setCurrentState(initialState);

        loadTemplates();
        connect(ui->cboTemplate, qOverload<int>(&QComboBox::currentIndexChanged), [this](int idx) {
            int itemIdx = ui->cboTemplate->itemData(idx,Qt::UserRole).toInt();
            loadSpecification(itemIdx);
        });

    }

    ~ImportTemplatePanelImpl()
    {
        delete ui;
    }

    const QString& name() const
    {
        return this->_name;
    }

    void regPanel(PanelActions *actions)
    {
        QAction *actTest = nullptr;
        QAction *actValidate = nullptr;
        QAction *actLoadSampleData = nullptr;

        actTest = actions->getAction(3);
        actValidate = actions->getAction(2);
        actLoadSampleData = actions->getAction(1);

        if(actTest)
            connect(actTest, &QAction::triggered, _panel, &ImportTemplatePanel::actionTestHandler);

        if(actValidate)
            connect(actValidate, &QAction::triggered, _panel, &ImportTemplatePanel::actionValidateHandler);

        if(actLoadSampleData)
            connect(actLoadSampleData, &QAction::triggered, _panel, &ImportTemplatePanel::actionLoadSampleHandler);
    }

    void unregPanel(PanelActions *actions)
    {
        for(int i: actions->getActionIdentifiers())
        {
            disconnect(actions->getAction(i), nullptr, _panel, nullptr);
        }
    }

    void activatePanel(PanelActions *actions, bool active)
    {
        if(active)
        {
            regPanel(actions);
            emit _panel->itemActionStateChange(_currentState);
        }
        else
            unregPanel(actions);
    }

    void itemActionHandler(ItemAction action)
    {
        switch(action)
        {
            case ItemAction::ADD:
            {
                QString name;
                Schema target;
                NewImportTemplateDialog *dialog = new NewImportTemplateDialog(&name, &target, nullptr);
                dialog->setModal(true);
                int r = dialog->exec();
                delete dialog;
                if(r == QDialog::Accepted)
                {
                    qDebug() << "Name: " << name << " Target: " << target.getName();
                    newTemplate(name,target);
                }
                break;
            }
            case ItemAction::EDIT:
            {
                int idx = ui->cboTemplate->currentData().toInt();
                if(idx != -1 && idx >= 0 && idx < _specs.length())
                {
                    _model->enableEditing(true);
                }
                break;
            }
            case ItemAction::SAVE:
            {
                _model->enableEditing(false);
                int idx = ui->cboTemplate->currentData().toInt();
                if(idx != -1 && idx >= 0 && idx < _specs.length())
                {
                    QMap<QString,QString> map = _model->getMap();
                    ImportSpecification spec = _specs.at(idx);
                    for(QString key: map.keys())
                    {
                        spec.setField(key,map[key]);
                    }

                    _specs[idx] = spec;

                    ImportSpecificationFactory::Save(_specs,Utility::GetUserPreferencesFilePath(APP::ApplicationImportTemplatesFile));
                }
                break;
            }
            case ItemAction::DELETE:
            {
                int idx = ui->cboTemplate->currentData().toInt();
                if(idx != -1 && idx >= 0 && idx < _specs.length())
                {
                    _specs.removeAt(idx);
                }
                break;
            }
            default:
                break;
        }
    }

    void registerItemStateNotifyFunction(void(ImportTemplatePanel::*func)(ItemState))
    {
        this->_notifyItemStateFunc = func;
    }

    ItemState getCurrentState() const
    {
        return this->_currentState;
    }

    void setCurrentState(ItemState state)
    {
        _currentState = state;
        notifyStateChange(state);
    }

    void loadSampleData(const QString& filepath)
    {
        QFile file = QFile(filepath);
        if(!file.open(QIODevice::ReadOnly))
        {
            ObjectError err("Unable to open file: " + file.errorString(), static_cast<int>(ObjectErrorCode::ERROR_READ_FILE));
            return;
        }
        _sample->load(&file, ',', true);
        file.close();
    }

private:
    ImportTemplatePanel *_panel;
    Ui::ImportTemplatePanel *ui;
    const QString _name = "Template";
    QList<ImportSpecification> _specs;
    TemplateMappingModel *_model;
    CSVModel *_sample;
    TemplateMappingDelegate *_delegate;
    ItemState _currentState;

    int _currentSpec;

    void(ImportTemplatePanel::*_notifyItemStateFunc)(ItemState) = nullptr;

    void notifyStateChange(ItemState state)
    {
        if(_notifyItemStateFunc != nullptr)
        {
            (_panel->*_notifyItemStateFunc)(state);
        }
    }

    void loadTemplates()
    {
        ui->cboTemplate->addItem("(Select A File Type)",-1);
        try
        {
            _specs = ImportSpecificationFactory::Load(
                    Utility::GetUserPreferencesFilePath(APP::ApplicationImportTemplatesFile));

            for(int i=0; i<_specs.count(); i++)
            {
                ui->cboTemplate->addItem(_specs[i].getName(), i);
            }
        }
        catch(ObjectError err)
        {
            if(err.errorCode() != (int)ObjectErrorCode::ERROR_NO_FILE)
            {
                QMessageBox::critical(_panel,"File Error" ,"Error opening template definition file");
            }
        }

        ui->cboTemplate->setCurrentIndex(0);
    }

    void newTemplate(const QString& name, const Schema& target)
    {
        ImportSpecification spec = ImportSpecification::initializeNew(name,target);
        _specs.append(spec);
        ui->cboTemplate->addItem(name, _specs.length()-1);
        ui->cboTemplate->setCurrentIndex(ui->cboTemplate->count()-1);
        loadSpecification(_specs.length()-1);
        _model->enableEditing(true);
    }

    void loadSpecification(int idx)
    {
        if(idx >= 0 && idx < _specs.count())
        {
            _currentSpec = idx;
            QMap<QString, QString> map;

            ImportSpecification spec = _specs[_currentSpec];
            QList<QString> fields = spec.getFields();

            for(int i=0; i<fields.length(); i++)
            {
                map.insert(fields[i],spec.getFieldExpression(fields[i]));
            }

            _model->enableEditing(false);
            _model->loadMapping(map);
        }
        else
        {
            _currentSpec = -1;
        }
    }
};

ImportTemplatePanel::ImportTemplatePanel(QWidget *parent) :
    PanelWindow(parent),
    impl(new ImportTemplatePanelImpl(this))
{
    impl->registerItemStateNotifyFunction(&ImportTemplatePanel::itemActionNotifyChange);
}

ImportTemplatePanel::~ImportTemplatePanel()
{}

void ImportTemplatePanel::itemActionHandler(ItemAction action)
{
    impl->itemActionHandler(action);
}

void ImportTemplatePanel::itemActionNotifyChange(ItemState change)
{
    emit itemActionStateChange(change);
}

ItemState ImportTemplatePanel::getCurrentState() const
{
    return impl->getCurrentState();
}

void ImportTemplatePanel::actionTestHandler()
{

}

void ImportTemplatePanel::actionValidateHandler()
{

}

QString ImportTemplatePanel::panelName() const
{
    return QString("Import Template Editor");
}

void ImportTemplatePanel::actionLoadSampleHandler()
{
    QStringList paths = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation);
    QString filename = QFileDialog::getOpenFileName(this,tr("Import CSV file"), (paths.size() > 0 ? paths[0]: ""),tr("Comma Separated Values File (*.csv);;All Files (*)"));
    if(!filename.isEmpty())
    {
        impl->loadSampleData(filename);
    }
}

Panel ImportTemplatePanel::panelId() const
{
    return Panel::IMPORT_TEMPLATE_EDITOR;
}

void ImportTemplatePanel::activate(PanelActions *actions)
{
    impl->activatePanel(actions, true);
}

void ImportTemplatePanel::deactivate(PanelActions *actions)
{
    impl->activatePanel(actions, false);
}
