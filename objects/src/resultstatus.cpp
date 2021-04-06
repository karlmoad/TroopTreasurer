#include <objects/resultstatus.h>

class ResultStatus::ResultStatusImpl
{
public:
    ResultStatusImpl(): _status(Status::UNKNOWN){}
    ResultStatusImpl(const ResultStatusImpl& copy)
    {
        this->_status = copy._status;
        this->_msg = copy._msg;
    }

    ~ResultStatusImpl(){}

    QString msg() const
    {
        return _msg;
    }

    Status status() const
    {
        return _status;
    }

    void setMsg(const QString& msg)
    {
        _msg = msg;
    }

    void setStatus(const Status status)
    {
        _status = status;
    }

private:
    QString _msg;
    Status _status;
};

ResultStatus::ResultStatus() : impl(new ResultStatusImpl)
{}

ResultStatus::ResultStatus(const ResultStatus &copy)
{
    impl.reset();
    impl = std::shared_ptr<ResultStatusImpl>(new ResultStatusImpl(*(copy.impl.get())));
}

ResultStatus::~ResultStatus()
{}

ResultStatus::Status ResultStatus::status()
{
    return impl->status();
}

QString ResultStatus::message()
{
    return impl->msg();
}

ResultStatus &ResultStatus::operator=(const ResultStatus &copy)
{
    if(*this != copy)
    {
        impl.reset();
        impl = std::shared_ptr<ResultStatusImpl>(new ResultStatusImpl(*(copy.impl.get())));
    }
    return *this;
}

bool ResultStatus::operator==(const ResultStatus &rhs) const
{
    return impl == rhs.impl;
}

bool ResultStatus::operator!=(const ResultStatus &rhs) const
{
    return !(rhs == *this);
}

ResultStatus::ResultStatus(std::shared_ptr<ResultStatusImpl> initialized)
{
    impl = initialized;
}

// Builder

struct ResultStatus::Builder::BuilderImpl
{
public:
    BuilderImpl(): _building(std::make_shared<ResultStatusImpl>()){}
    std::shared_ptr<ResultStatusImpl> _building;
};

ResultStatus::Builder::Builder(): builder(std::make_shared<BuilderImpl>())
{}

ResultStatus::Builder::~Builder()
{}

ResultStatus::Builder &ResultStatus::Builder::setStatus(ResultStatus::Status status)
{
    builder->_building->setStatus(status);
    return *this;
}

ResultStatus::Builder &ResultStatus::Builder::setMessage(const QString &message)
{
    builder->_building->setMsg(message);
    return *this;
}

ResultStatus ResultStatus::Builder::build()
{
    return ResultStatus(builder->_building);
}
