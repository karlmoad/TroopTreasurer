/*
 * KeyManager
 * Author: Karl Moad
 * ---------------------------------
 * KeyManagerError - Error communication class
 */

#include "keymanager/keymanager.h"
KeyManagerError::KeyManagerError(int code, QString message): _code(code), _msg(message)
{}

int KeyManagerError::getErrorCode() const
{
    return _code;
}

QString KeyManagerError::getErrorMessage() const
{
    return _msg;
}

