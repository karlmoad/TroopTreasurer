#ifndef TROOPTREASURER_DATAIMPORTCONTROLLER_H
#define TROOPTREASURER_DATAIMPORTCONTROLLER_H

#include <QtCore>
#include <memory>
#include <QAbstractTableModel>
#include "importspecificationruntime.h"
#include "importspecification.h"
#include "schema.h"
#include "objecterror.h"


class DataImportController: public QObject
{
    Q_OBJECT
public:
    virtual ~DataImportController();
    DataImportController(const DataImportController&) = delete;
    DataImportController(DataImportController&&) = delete;
    DataImportController& operator=(const DataImportController&) = delete;
    DataImportController& operator=(DataImportController&&) = delete;

    void start();

signals:
    void notifyProgress(int idx, const QJsonObject& data);
    void notifyCompletion(int processed, int successful, int duplicates, int errors);

public:
    class Builder
    {
    public:
        explicit Builder(QObject *parent= nullptr);
        virtual ~Builder();
        Builder(const Builder&) = delete;
        Builder(Builder&&) = delete;
        Builder& operator=(const Builder&) = delete;
        Builder& operator=(Builder&&) = delete;

        Builder& setModel(QAbstractTableModel *input);
        Builder& setSpecification(ImportSpecification *spec);
        Builder& setSchema(const Schema& schema);
        Builder& setOptionDuplicateCheck(bool option);
        Builder& setOptionTruncate(bool option);
        DataImportController* build();

    private:
        DataImportController *_instance;
    };

protected:
    explicit DataImportController(QObject *parent=nullptr);

private:
    class DataImportControllerImpl;
    std::shared_ptr<DataImportControllerImpl> impl;
};


#endif //TROOPTREASURER_DATAIMPORTCONTROLLER_H
