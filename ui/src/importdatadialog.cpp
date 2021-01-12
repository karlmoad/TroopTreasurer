#include "ui/importdatadialog.h"
#include "ui_importdatadialog.h"
#include <QDebug>

class ImportDataDialog::ImportDataDialogImpl
{
public:
    ImportDataDialogImpl(ImportDataDialog *dialog): _ui(new Ui::ImportDataDialog), _dialog(dialog)
    {
        _ui->setupUi(_dialog);
        connect(_ui->btnCancel, &QPushButton::clicked, _dialog,  &ImportDataDialog::cancelHandler);
        connect(_ui->btnNext, &QPushButton::clicked, _dialog, &ImportDataDialog::nextHandler);
        connect(_ui->btnRun, &QPushButton::clicked, _dialog, &ImportDataDialog::executeHandler);
        connect(_ui->paneStack,&QStackedWidget::currentChanged, _dialog, &ImportDataDialog::activePaneChangedHandler);
        _ui->paneStack->setCurrentIndex(0);
    }

    ~ImportDataDialogImpl()
    {
        delete _ui;
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
        _ui->paneStack->setCurrentIndex(1);
    }

    void execute()
    {
        qDebug() << "Get ready for a surprise";
    }

private:
    Ui::ImportDataDialog *_ui;
    ImportDataDialog *_dialog;

};

ImportDataDialog::ImportDataDialog(QWidget *parent) :
    QDialog(parent),
    impl(new ImportDataDialogImpl(this))
{

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
