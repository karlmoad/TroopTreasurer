#ifndef TROOPTREASURER_RESULTSTATUS_H
#define TROOPTREASURER_RESULTSTATUS_H

#include <QString>
#include <memory>
#include "objects.h"

class ResultStatus
{
public:
    ResultStatus();
    ResultStatus(const ResultStatus &copy);
    virtual ~ResultStatus();

    bool status();
    bool isError();
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
        Builder& setStatus(bool status);
        Builder& setIsError(bool error);
        Builder& setMessage(const QString &message);
        ResultStatus build();
    private:
        class BuilderImpl;
        std::shared_ptr<BuilderImpl> _building;
    };

private:
    bool _status;
    bool _error;
    QString _msg;
};


#endif //TROOPTREASURER_RESULTSTATUS_H
