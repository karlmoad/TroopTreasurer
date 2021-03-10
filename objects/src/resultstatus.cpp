#include <objects/resultstatus.h>

ResultStatus::ResultStatus() : _status(false), _error(false), _msg(QString())
{}

ResultStatus::ResultStatus(const ResultStatus &copy)
{
    this->_status = copy._status;
    this->_error = copy._error;
    this->_msg = copy._msg;
}

ResultStatus::~ResultStatus()
{

}

bool ResultStatus::status()
{
    return this->_status
}

bool ResultStatus::isError()
{
    return this->_error;
}

QString ResultStatus::message()
{
    return this->_msg;
}

ResultStatus &ResultStatus::operator=(const ResultStatus &copy)
{
    if(*this != copy)
    {
        this->_status = copy._status;
        this->_error = copy._error;
        this->_msg = copy._msg;
    }
    return *this;
}

bool ResultStatus::operator==(const ResultStatus &rhs) const
{
    return _status == rhs._status &&
           _error == rhs._error &&
           _msg.compare(rhs._msg, Qt::CaseSensitive) == 0;
}

bool ResultStatus::operator!=(const ResultStatus &rhs) const
{
    return !(rhs == *this);
}

// Buidler

class ResultStatus::Builder::BuilderImpl
{
public:
    ResultStatus _constructing;
};

ResultStatus::Builder::Builder()
{
    _building = std::shared_ptr<BuilderImpl>(new BuilderImpl());
}

ResultStatus::Builder::~Builder()
{}

ResultStatus::Builder &ResultStatus::Builder::setStatus(bool status)
{
    this->_building->_constructing._status = status;
    return *this;
}

ResultStatus::Builder &ResultStatus::Builder::setIsError(bool error)
{
    this->_building->_constructing._error = error;
    return *this;
}

ResultStatus::Builder &ResultStatus::Builder::setMessage(const QString &message)
{
    this->_building->_constructing._msg = message;
    return *this;
}

ResultStatus ResultStatus::Builder::build()
{
    return this->_building->_constructing;
}
