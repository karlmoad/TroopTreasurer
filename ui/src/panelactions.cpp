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
    PanelActionsImpl(QMenuBar *menubar, QToolBar *toolbar,const QJsonObject& definition, PanelActions *clazz): _clazz(clazz), _def(definition)
    {
        _menu = nullptr;
        initialize(menubar,toolbar);
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

    void setVisibility(bool visible)
    {
        if(_menu)
        {
            _menu->setVisible(visible);
        }

        for(int i : _actions.keys())
        {
            _actions[i]->setVisible(visible);
        }
    }

private:
    void initialize(QMenuBar *menubar, QToolBar *toolbar)
    {
        // All actions and/or menus are created and set into a non visible state
        bool hasMenu = false;
        if(_def.isEmpty()) return;

        _panel = static_cast<Panel>(_def["id"].toInt(0));

        if(_panel== Panel::UNDEFINED) return; // something is wrong with config, if panel id 0 then it is undefined so stop here

        //create a main menu entry if configured
        if(_def.contains("menu"))
        {
            hasMenu = true;
            _menu = menubar->addMenu(_def["menu"].toString());
            _menu->setVisible(false);
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
                    action = new QAction(QIcon(iicon),ititle,_clazz);
                }
                else
                {
                    action = new QAction(ititle,_clazz);
                }

                if(action)
                {
                    action->setVisible(false);
                    if (hasMenu && _menu && imenu)
                    {
                        _menu->addAction(action);
                    }

                    if (toolbar && itbar)
                    {
                        toolbar->addAction(action);
                    }

                    _actions.insert(iid, action);
                }
            }
        }

    }

private:
    QMenu *_menu;
    QMap<int, QAction*> _actions;
    Panel _panel;
    PanelActions *_clazz;
    QJsonObject _def;
};

PanelActions::PanelActions(QMenuBar *menubar, QToolBar *toolbar, const QJsonObject& definition, QObject *parent) :
                                                                                            QObject(parent),
                                                                                            impl(new PanelActionsImpl(menubar,toolbar,definition,this))
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

QList<PanelActions *> PanelActions::LoadPanelActionDefinitions(QMenuBar *menubar, QToolBar *toolbar, QObject *parent)
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
            PanelActions *pa = new PanelActions(menubar,toolbar,panel,parent);
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
