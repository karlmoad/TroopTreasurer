#include "ui/databasesettingspane.h"
#include "ui_databasesettingspane.h"
#include "objects/settingsmodel.h"
#include <QDebug>

class DatabaseSettingsPane::DatabaseSettingsPaneImpl
{
public:
    DatabaseSettingsPaneImpl(DatabaseSettingsPane *pane): _pane(pane), _ui(new Ui::DatabaseSettingsPane)
    {
        _ui->setupUi(_pane);
        _model = new SettingsModel();
        _delegate = new SettingsModelDelegate();
        _ui->tblSettings->setModel(_model);
        _ui->tblSettings->setItemDelegate(_delegate);
        connect(_ui->btnTest, &QPushButton::clicked, _pane, &DatabaseSettingsPane::testConnectionSettings);
    }

    ~DatabaseSettingsPaneImpl()
    {
        delete _ui;
    }

    void loadSettings(const QJsonObject& meta, const QJsonObject& settings)
    {
        _model->loadSettings((meta.contains("parameters") ? meta["parameters"].toArray():QJsonArray()),settings);
    }

    void setTestConnectionMessage(const QString& msg)
    {
        _ui->lblMessage->setText(msg);
    }

    void testConnection()
    {
        QJsonObject _zettings = _model->getSettings();
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL","TEST");
        QString pwdKey = QString("%1%2").arg(_zettings["host"].toString(""), _zettings["user"].toString(""));

        if (pwdKey.trimmed().size() > 0)
        {
            db.setDatabaseName(_zettings["schema"].toString(""));
            db.setHostName(_zettings["host"].toString(""));
            db.setPort(_zettings["port"].toInt(3306));
            db.setUserName(_zettings["user"].toString(""));
            db.setPassword(_zettings["password"].toString(""));
            QString opts;
            opts.append(QString("SSL_CA=%1;").arg(_zettings["ca_crt"].toString("")));
            opts.append(QString("SSL_CERT=%1;").arg(_zettings["svr_crt"].toString("")));
            opts.append(QString("SSL_KEY=%1;").arg(_zettings["svr_key"].toString("")));
            db.setConnectOptions(opts);

            bool opened = db.open();
            if (opened)
            {
                setTestConnectionMessage("Connection Successful");
                db.close();
            }
            else
            {
                setTestConnectionMessage("Error: " + db.lastError().text());
            }
        }
        else
        {
            setTestConnectionMessage("Insufficient settings provided");
        }
        QSqlDatabase::removeDatabase("TEST");
    }

    QJsonObject getSettings()
    {
        return _model->getSettings();
    }

    void resize()
    {
        int controlWidth = _ui->tblSettings->width();
        controlWidth -= 0;
        auto width =controlWidth *.5;
        _ui->tblSettings->horizontalHeader()->resizeSection(0,width);
        _ui->tblSettings->horizontalHeader()->resizeSection(1,width);
    }
private:
    DatabaseSettingsPane *_pane;
    Ui::DatabaseSettingsPane *_ui;
    SettingsModel *_model;
    SettingsModelDelegate *_delegate;
};

DatabaseSettingsPane::DatabaseSettingsPane(QWidget *parent) :
    QWidget(parent),
    impl(new DatabaseSettingsPaneImpl(this))
{}

DatabaseSettingsPane::~DatabaseSettingsPane()
{
    delete impl;
}

ApplicationSettingsType DatabaseSettingsPane::settingsType()
{
    return ApplicationSettingsType::DATABASE;
}

void DatabaseSettingsPane::loadSettings(const QJsonObject& meta, const QJsonObject& settings)
{
    impl->loadSettings(meta, settings);
}

QJsonObject DatabaseSettingsPane::settings()
{
    return impl->getSettings();
}

void DatabaseSettingsPane::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    impl->resize();
}

void DatabaseSettingsPane::testConnectionSettings()
{
    impl->testConnection();
}
