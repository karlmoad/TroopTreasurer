#include <QString>
#include "objects/importspecification.h"
#include "objects/importspecificationruntime.h"
#include "objects/utility.h"
#include "objects/templatemappingmodel.h"
#include "ui/importtemplatepanel.h"
#include "ui_importtemplatepanel.h"
#include "ui/applicationconstants.h"
#include <QDebug>

class ImportTemplatePanel::ImportTemplatePanelImpl
{
public:
    ImportTemplatePanelImpl(ImportTemplatePanel *panel): ui(new Ui::ImportTemplatePanel)
    {
        this->_panel = panel;
        ui->setupUi(_panel);
        _actTest = new QAction(QIcon(":/resources/bug.png"), "Test Template", panel);
        _actValidate = new QAction(QIcon(":/resources/page_white_gear.png"), "Validate Template", panel);
        _actTest->setStatusTip("Test the template function");
        _actValidate->setStatusTip("Validate template definition");
        _menuItems.append(_actValidate);
        _menuItems.append(_actTest);
        _toolbarItems.append(_actValidate);
        _toolbarItems.append(_actTest);

        _model = new TemplateMappingModel(_panel);
        _delegate = new TemplateMappingDelegate(_panel);
        ui->tableTemplate->setModel(_model);
        ui->tableTemplate->setItemDelegate(_delegate);

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

    const QList<QAction *>& menuItems() const
    {
        return this->_menuItems;
    }

    const QList<QAction *>& toolbarItems() const
    {
        return this->_toolbarItems;
    }

    void itemActionHandler(ItemAction action)
    {
        switch(action)
        {
            case ItemAction::ADD:

                break;
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

private:
    ImportTemplatePanel *_panel;
    Ui::ImportTemplatePanel *ui;
    const QString _name = "Template";
    QAction *_actTest;
    QAction *_actValidate;
    QList<QAction *> _menuItems;
    QList<QAction *> _toolbarItems;
    QList<ImportSpecification> _specs;
    TemplateMappingModel *_model;
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
        _specs = ImportSpecificationFactory::Load(Utility::GetUserPreferencesFilePath(APP::ApplicationImportTemplatesFile));
        for(int i=0; i<_specs.count(); i++)
        {
            ui->cboTemplate->addItem(_specs[i].getName(), i);
        }
        ui->cboTemplate->setCurrentIndex(0);
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
{

    delete impl;
}

const QString &ImportTemplatePanel::panelMenuText() const
{
    return impl->name();
}

const QList<QAction *>& ImportTemplatePanel::menuItems() const
{
    return impl->menuItems();
}

const QList<QAction *>& ImportTemplatePanel::toolbarItems() const
{
    return impl->toolbarItems();
}

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

bool ImportTemplatePanel::hasMenu() const
{
    return true;
}

bool ImportTemplatePanel::hasToolbarItems() const
{
    return true;
}

void ImportTemplatePanel::activate()
{

}
