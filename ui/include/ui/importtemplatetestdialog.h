//
// Created by Karl Moad on 12/28/20.
//

#ifndef TROOPTREASURER_IMPORTTEMPLATETESTDIALOG_H
#define TROOPTREASURER_IMPORTTEMPLATETESTDIALOG_H

#include <QtWidgets>
#include <memory>
#include <QAbstractTableModel>
#include "objects/importspecification.h"

class ImportTemplateTestDialog : public QDialog
{
Q_OBJECT

public:
    explicit ImportTemplateTestDialog(QAbstractTableModel *model, ImportSpecification *spec, QWidget *parent=nullptr);
    explicit ImportTemplateTestDialog(QWidget *parent = nullptr);
    ~ImportTemplateTestDialog() override;

    void setModel(QAbstractTableModel *model);
    void setSpecification(ImportSpecification *spec);

    void load();

private:
    class ImportTemplateTestDialogImpl;
    std::shared_ptr<ImportTemplateTestDialogImpl> impl;
};

#endif //TROOPTREASURER_IMPORTTEMPLATETESTDIALOG_H
