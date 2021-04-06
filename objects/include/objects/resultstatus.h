#ifndef TROOPTREASURER_RESULTSTATUS_H
#define TROOPTREASURER_RESULTSTATUS_H

#include <QString>
#include <memory>
#include "objects.h"

class ResultStatus
{
public:
    enum class Status{UNKNOWN=0, SUCCESS, ERROR};

    ResultStatus();
    ResultStatus(const ResultStatus &copy);
    virtual ~ResultStatus();

    Status status();
    QString message();

    ResultStatus& operator=(const ResultStatus &copy);

    bool operator==(const ResultStatus &rhs) const;

    bool operator!=(const ResultStatus &rhs) const;

    class Builder
    {
    public:
        REM_COPY_MOVE(Builder)
        Builder();
        ~Builder();
        Builder& setStatus(Status status);
        Builder& setMessage(const QString &message);
        ResultStatus build();
    private:
        class BuilderImpl;
        std::shared_ptr<BuilderImpl> builder;
    };

private:
    class ResultStatusImpl;
    ResultStatus(std::shared_ptr<ResultStatusImpl> initialized);
    std::shared_ptr<ResultStatusImpl> impl;
};


#endif //TROOPTREASURER_RESULTSTATUS_H
