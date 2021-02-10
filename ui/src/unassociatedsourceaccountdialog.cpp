#include "ui/unassociatedsourceaccountdialog.h"
#include "ui_unassociatedsourceaccountdialog.h"
#include <QListWidgetItem>

class UnassociatedSourceAccountDialog::UnassociatedSourceAccountDialogImpl
{
public:
    UnassociatedSourceAccountDialogImpl(UnassociatedSourceAccountDialog *dialog): _dialog(dialog), _ui(new Ui::UnassociatedSourceAccountDialog)
    {
        _ui->setupUi(_dialog);
        connect(_ui->btnClose, &QPushButton::clicked, _dialog, &UnassociatedSourceAccountDialog::closeHandler);
        connect(_ui->btnCreate, &QPushButton::clicked, _dialog, &UnassociatedSourceAccountDialog::createHandler);
    }

    ~UnassociatedSourceAccountDialogImpl()
    {
        delete _ui;
    }

    void load(const QList<QJsonObject>& data)
    {
        for(int i=0; i<data.length(); i++)
        {
            QJsonObject json = data.at(i);
            _ui->listItems->addItem(json["name"].toString());
            QListWidgetItem *item = _ui->listItems->item(_ui->listItems->count()-1);
            item->setData(Qt::UserRole, json);
        }
    }

    QJsonObject getSelected()
    {
        return _selected;
    }

    bool setSelected()
    {
        auto selected =_ui->listItems->selectedItems();
        if(selected.count() == 1)
        {
            _selected = selected.at(0)->data(Qt::UserRole).toJsonObject();
            return true;
        }
        return false;
    }

private:
    UnassociatedSourceAccountDialog *_dialog;
    Ui::UnassociatedSourceAccountDialog *_ui;
    QJsonObject _selected;
};

UnassociatedSourceAccountDialog::UnassociatedSourceAccountDialog(QWidget *parent) :
    QDialog(parent), impl(new UnassociatedSourceAccountDialogImpl(this))
{}

UnassociatedSourceAccountDialog::~UnassociatedSourceAccountDialog()
{}

void UnassociatedSourceAccountDialog::setData(const QList<QJsonObject> &data)
{
    impl->load(data);
}

void UnassociatedSourceAccountDialog::closeHandler()
{
    this->done(QDialog::Rejected);
    this->close();
}

void UnassociatedSourceAccountDialog::createHandler()
{
    if(impl->setSelected())
    {
        this->done(QDialog::Accepted);
        this->close();
    }
}

QJsonObject UnassociatedSourceAccountDialog::getSelected()
{
    return impl->getSelected();
}
