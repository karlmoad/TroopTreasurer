#include <QString>
#include <ui/importtemplatepanel.h>
#include "ui/importtemplatepanel.h"
#include "ui_importtemplatepanel.h"
#include <QDebug>

class ImportTemplatePanel::ImportTemplatePanelImpl
{
public:
    ImportTemplatePanelImpl(ImportTemplatePanel *panel)
    {
        this->_panel = panel;
        _actTest = new QAction(QIcon(":/resources/bug.png"), "Test Template", panel);
        _actValidate = new QAction(QIcon(":/resources/page_white_gear.png"), "Validate Template", panel);
        _actTest->setStatusTip("Test the templates function");
        _actValidate->setStatusTip("Validate template definition");
        _menuItems.append(_actValidate);
        _menuItems.append(_actTest);
        _toolbarItems.append(_actValidate);
        _toolbarItems.append(_actTest);
    }

    ~ImportTemplatePanelImpl()
    {

    }

    void setListReference(QListWidget *list)
    {
        _list = list;
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
                _list->addItem("Add action received");
                break;
            case ItemAction::EDIT:
                _list->addItem("Edit action received");
                break;
            case ItemAction::SAVE:
                _list->addItem("Save action received");
                break;
            case ItemAction::DELETE:
                _list->addItem("Delete action received");
                break;
            default:
                _list->addItem("Unknown action received");
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
    QListWidget *_list;
    const QString _name = "Template";
    QAction *_actTest;
    QAction *_actValidate;
    QList<QAction *> _menuItems;
    QList<QAction *> _toolbarItems;
    ItemState _currentState;

    void(ImportTemplatePanel::*_notifyItemStateFunc)(ItemState) = nullptr;

    void notifyStateChange(ItemState state)
    {
        if(_notifyItemStateFunc != nullptr)
        {
            (_panel->*_notifyItemStateFunc)(state);
        }
    }
};

ImportTemplatePanel::ImportTemplatePanel(QWidget *parent) :
    PanelWindow(parent),
    ui(new Ui::ImportTemplatePanel),
    impl(new ImportTemplatePanelImpl(this))
{
    ui->setupUi(this);
    impl->registerItemStateNotifyFunction(&ImportTemplatePanel::itemActionNotifyChange);
    impl->setListReference(ui->lstMessages);
    connect(ui->btnSend, &QPushButton::clicked, this , &ImportTemplatePanel::sendUpdateButtonHandler);
    ui->chkNew->setCheckState(Qt::Checked);
    ui->chkEdit->setCheckState(Qt::Unchecked);
    ui->chkDelete->setCheckState(Qt::Unchecked);
    ui->chkSave->setCheckState(Qt::Unchecked);
}

ImportTemplatePanel::~ImportTemplatePanel()
{
    delete ui;
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
    ui->lstMessages->addItem("Test Handler Activated");
}

void ImportTemplatePanel::actionValidateHandler()
{
    ui->lstMessages->addItem("Validate Handler Activated");
}

void ImportTemplatePanel::sendUpdateButtonHandler()
{
    sendCurrentSelectionState();
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
    sendCurrentSelectionState();
}

void ImportTemplatePanel::sendCurrentSelectionState()
{
    ItemState s;
    s.setSaveEnabled(ui->chkSave->checkState() == Qt::Checked);
    s.setEditEnabled(ui->chkEdit->checkState() == Qt::Checked);
    s.setAddEnabled(ui->chkNew->checkState() == Qt::Checked);
    s.setDeleteEnabled(ui->chkDelete->checkState() == Qt::Checked);
    impl->setCurrentState(s);
}
