/*
 * KeyManager
 * Author: Karl Moad
 * ---------------------------------
 * KeyManager - interface for local system keystore
 * abstracts away the localized implementation of
 * keystore value storage for secure values
 *
 *  Apple OSX - KeyChain
 *  Windows 7 > - Windows Credential store
 */

#ifndef KOAD_IO_KEYMANAGER_H
#define KOAD_IO_KEYMANAGER_H

#include <QString>

enum class KeyManagerErrorCodes
{
    UNABLE_TO_RETRIEVE =100,
    UNABLE_TO_SAVE=200,
    UNABLE_TO_ADD=300,
    UNABLE_TO_DECRYPT=400,
    KEY_SIZE_ERROR = 500,
    CREDENTIAL_SIZE_ERROR=600
};

class KeyManagerError
{
public:
    KeyManagerError(int code, QString message);
    ~KeyManagerError()=default;
     int getErrorCode() const;
     QString getErrorMessage() const;

private:
    int _code;
    QString _msg;
};

class KeyManager
{
public:
    explicit KeyManager();
    virtual ~KeyManager();

    KeyManager(const KeyManager&) = delete;
    KeyManager& operator=(const KeyManager&) = delete;

    virtual bool getValue(const QString& key, QString& value);
    virtual bool setValue(const QString& key, const QString& value);

    KeyManagerError lastError();

protected:
    class KeyManagerImpl;
    KeyManagerImpl *impl;
};

#endif
