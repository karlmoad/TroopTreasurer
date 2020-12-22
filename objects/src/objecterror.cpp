#include "objects/objecterror.h"


ObjectError::ObjectError(const QString &text, int code) noexcept : _message(text), _errorCode(code)
{}

ObjectError::ObjectError(const ObjectError &re)
{
    this->_message = re._message;
    this->_errorCode = re._errorCode;
}

ObjectError::~ObjectError()
{}

void ObjectError::raise()
{
    throw *this;
}

ObjectError *ObjectError::clone() const
{
    return new ObjectError(*this);
}

const char *ObjectError::what() const noexcept
{
    return this->_message.toStdString().c_str();
}

int ObjectError::errorCode() const
{
    return this->_errorCode;
}

