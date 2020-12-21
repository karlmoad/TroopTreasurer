#include "ui/mainsettingspane.h"
#include "ui_mainsettingspane.h"

class MainSettingsPane::MainSettingsPaneImpl
{
public:
    MainSettingsPaneImpl(MainSettingsPane *pane)
    {
        _pane = pane;
        _ui = new Ui::MainSettingsPane();
        _ui->setupUi(_pane);
        init();
    }

    ~MainSettingsPaneImpl()
    {
        delete _ui;
    }

    Ui::MainSettingsPane *_ui;
    MainSettingsPane *_pane;

private:
    void init()
    {
        QList<ApplicationSettingsType> sections = ApplicationSettingsUtility::ApplicationSettingsTypeList();
        for(int i = 0; i< sections.count(); i++)
        {
            QListWidgetItem *item = new QListWidgetItem();
            item->setIcon(ApplicationSettingsUtility::ApplicationSettingsTypeIcon(sections[i]));
            item->setText(ApplicationSettingsUtility::ApplicationSettingTypeToString(sections[i]));
            item->setData(Qt::UserRole, static_cast<int>(sections[i]));
            _ui->lstPanes->addItem(item);
        }
        connect(_ui->lstPanes, &QListWidget::itemDoubleClicked, _pane, &MainSettingsPane::paneListItemClicked);
    }
};

MainSettingsPane::MainSettingsPane(QWidget *parent) :
    QWidget(parent),
    impl(new MainSettingsPaneImpl(this))
{
    connect(impl->_ui->lstPanes, &QListWidget::itemDoubleClicked, this, &MainSettingsPane::paneListItemClicked);
}

MainSettingsPane::~MainSettingsPane()
{
    delete impl;
}

void MainSettingsPane::paneListItemClicked(QListWidgetItem *item)
{
    emit settingsSectionSelected(static_cast<ApplicationSettingsType>(item->data(Qt::UserRole).toInt()));
}
