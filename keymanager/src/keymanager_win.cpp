/*
 * KeyManager
 * Author: Karl Moad
 * ---------------------------------
 * KeyManager - Windows implementation
 */

#include <windows.h>
#include <wincrypt.h>
#include <wincred.h>
#include "keymanager/keymanager.h"
#include "keymanager_impl.h"

KeyManager::KeyManager(const QString &AppId): impl(new KeyManagerImpl(AppId))
{}

KeyManager::~KeyManager()
{
    delete impl;
}

bool KeyManager::getValue(const QString &key, QString &value)
{
    impl->resetLastError();
    LPCWSTR lkey = (LPCWSTR)impl->generateKeyStorageKey(key).utf16();
    PCREDENTIALW cred;

    if(!CredReadW(lkey, CRED_TYPE_GENERIC, 0, &cred))
    {
        switch(GetLastError())
        {
            case ERROR_NOT_FOUND:
                impl->setError(static_cast<int>(KeyManagerErrorCodes::UNABLE_TO_RETRIEVE), "Password Entry Not Found");
                break;
            default:
                impl->setError(static_cast<int>(KeyManagerErrorCodes::UNABLE_TO_DECRYPT), "Could not decrypt password");

        }
        return false;
    }

    value = QString(QByteArray((char*)cred->CredentialBlob, cred->CredentialBlobSize));
    CredFree(cred);
    return true;
}

bool KeyManager::setValue(const QString &key, const QString &value)
{
    impl->restLastError();

    CREDENTIALW cred;
    char *val = value.toUtf8().data();
    LPWSTR key = (LPCWSTR)impl->generateKeyStorageKey(key).utf16();
    LPWSTR acct = (LPWSTR)key.utf16();

    memset(&cred, 0, sizeof(cred));
    cred.Comment = const_cast<wchar_t*>(L"koad_io_KeyManager");
    cred.Type = CRED_TYPE_GENERIC;
    cred.TargetName = key;
    cred.UserName = acct;
    cred.CredentialBlobSize = value.size();
    cred.CredentialBlob = (LPBYTE)val;
    cred.Persist = CRED_PERSIST_ENTERPRISE;

    if (CredWriteW(&cred, 0)) {
        return true;
    }
    DWORD err = GetLastError();
    switch(err) {

        // Detect size-exceeded errors and provide nicer messages.
        // Unfortunately these error codes aren't documented.
        // Found empirically on Win10 1803 build 17134.523.
        case RPC_X_BAD_STUB_DATA:
        {
            const size_t maxBlob = CRED_MAX_CREDENTIAL_BLOB_SIZE;
            impl->setError(static_cast<int>(KeyManagerErrorCodes::CREDENTIAL_SIZE_ERROR),QString("Credential size exceeds maximum size of %1").arg(maxBlob));
            return false;
        }
        case RPC_S_INVALID_BOUND: {
            const size_t maxTargetName = CRED_MAX_GENERIC_TARGET_NAME_LENGTH;
            impl->setError(static_cast<int>(KeyManagerErrorCodes::KEY_SIZE_ERROR), QString("Credential key exceeds maximum size of %1").arg(maxTargetName));
            return false
        }
        default:
        {
            impl->setError(static_cast<int>(KeyManagerErrorCodes::UNABLE_TO_SAVE), QString("Failed to persist password into keystore, win32 error code: %1").arg(err));
            return false;
        }
    }
}

KeyManagerError KeyManager::lastError()
{
    return impl->currentError();
}
