#include "ui/panelactions.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "objects/objecterror.h"

class PanelActions::PanelActionsImpl
{
public:
    PanelActionsImpl(const QJsonObject& definition, QMainWindow *window): _window(window), _def(definition)
    {
        _menu = nullptr;
        _toolbar = nullptr;
        initialize();
    }

    ~PanelActionsImpl()
    {}

    Panel panel()
    {
        return _panel;
    }

    QList<int> actionIdentifiers()
    {
        return _actions.keys();
    }

    QAction* action(int id)
    {
        if(_actions.contains(id))
        {
            return _actions[id];
        }
        return nullptr;
    }

    bool hasMenu() const
    {
        return _hasMenu;
    }

    bool hasToolbar() const
    {
        return _hasToolbar;
    }

    QMenu* menu() const
    {
        return _menu;
    }

    QToolBar* toolbar() const
    {
        return _toolbar;
    }

    void setVisibility(bool visible)
    {
        if(_menu)
        {
            _menu->menuAction()->setVisible(visible);
        }

        if(_toolbar)
        {
            _toolbar->setVisible(visible);
        }
    }

private:
    void initialize()
    {
        // All actions and/or menus are created and set into a non visible state
        if(_def.isEmpty()) return;
        _panel = static_cast<Panel>(_def["id"].toInt(0));

        if(_panel== Panel::UNDEFINED) return; // something is wrong with config, if panel id 0 then it is undefined so stop here

        _hasMenu=_def["menu"].toBool(false);
        _hasToolbar=_def["toolbar"].toBool(false);
        _displayText=_def["display"].toString(QString());

        //create a main menu entry if configured
        if(_hasMenu)
        {
            _menu = _window->menuBar()->addMenu(_displayText);
            _menu->menuAction()->setVisible(false);
        }

        if(_hasToolbar)
        {
            _toolbar = _window->addToolBar(_displayText);
            _toolbar->setVisible(false);
        }

        if(_def.contains("items"))
        {
            QJsonArray items = _def["items"].toArray();
            for(int i =0; i< items.count(); i++)
            {
                QJsonObject item = items[i].toObject();
                int iid = item["id"].toInt(0);
                bool imenu = item["menu"].toBool(false);
                bool itbar = item["toolbar"].toBool(false);
                QString iicon = item["icon"].toString(QString());
                QString ititle = item["title"].toString(QString());

                QAction *action=nullptr;

                if(iicon.length() > 0)
                {
                    action = new QAction(QIcon(iicon),ititle,_window);
                }
                else
                {
                    action = new QAction(ititle,_window);
                }

                if(action)
                {
                    if (_hasMenu && _menu && imenu)
                    {
                        _menu->addAction(action);
                    }

                    if (_toolbar && _toolbar && itbar)
                    {
                        _toolbar->addAction(action);
                    }
                    _actions.insert(iid, action);
                }
            }
        }
    }

private:
    QMenu *_menu;
    QToolBar *_toolbar;
    QMap<int, QAction*> _actions;
    Panel _panel;
    QMainWindow *_window;
    QJsonObject _def;
    QString _displayText;
    bool _hasMenu;
    bool _hasToolbar;
};

PanelActions::PanelActions(const QJsonObject& definition, QMainWindow *window) :impl(new PanelActionsImpl(definition,window))
{}

PanelActions::~PanelActions()
{}

Panel PanelActions::getPanel()
{
    return impl->panel();
}

QAction *PanelActions::getAction(int id)
{
    return impl->action(id);
}

void PanelActions::setVisibility(bool visible)
{
    impl->setVisibility(visible);
}

QList<PanelActions *> PanelActions::LoadPanelActionDefinitions(QMainWindow *window)
{
    QList<PanelActions*> out;

    //create and register panel main menu entries
    QFile file(":/resources/files/panels.json");
    if(file.open(QFile::ReadOnly | QFile::Text)){
        QByteArray raw = file.readAll();
        file.close();
        QJsonObject panelConfig = QJsonDocument::fromJson(raw).object();
        QJsonArray panels = panelConfig["panels"].toArray();
        for(int i=0; i<panels.count(); i++)
        {
            QJsonObject panel = panels[i].toObject();
            PanelActions *pa = new PanelActions(panel,window);
            out.append(pa);
        }
    }
    else
    {
        ObjectError err("Unable to open and read main panel registration file", static_cast<int>(ObjectErrorCode::ERROR_READ_FILE));
        err.raise();
    }
    return out;
}

QList<int> PanelActions::getActionIdentifiers()
{
    return impl->actionIdentifiers();
}

bool PanelActions::hasMenu() const
{
    return impl->hasMenu();
}

QMenu *PanelActions::getMenu() const
{
    return impl->menu();
}

bool PanelActions::hasToolbar() const
{
    return impl->hasToolbar();
}

QToolBar *PanelActions::getToolbar() const
{
    return impl->toolbar();
}
