/*
 * KeyManager
 * Author: Karl Moad
 * ---------------------------------
 * KeyManager - OSX implementation
 */

#include <Security/Security.h>
#include "keymanager/keymanager.h"
#include "keymanager_impl.h"

KeyManager::KeyManager(): impl(new KeyManagerImpl())
{}

KeyManager::~KeyManager()
{}

bool KeyManager::getValue(const QString &key, QString &value)
{
    impl->resetLastError();
    UInt32 value_length;
    char* value_char_array;
    OSStatus ret = SecKeychainFindGenericPassword(
            NULL,
            key.length(),
            key.toStdString().c_str(),
            key.length(),
            key.toStdString().c_str(),
            &value_length,
            (void**)&value_char_array,
            NULL);

    if (ret == 0) {
        value = QString::fromLatin1(value_char_array, value_length);
        SecKeychainItemFreeContent(NULL, value_char_array);
        return true;
    }else
    {
        impl->setError(static_cast<int>(KeyManagerErrorCodes::UNABLE_TO_RETRIEVE), "Unable to retrieve value for key.");
        return false;
    }
}

bool KeyManager::setValue(const QString &key, const QString &value)
{
    impl->resetLastError();
    SecKeychainItemRef item;
    OSStatus ret = SecKeychainFindGenericPassword(
            NULL,
            key.length(),
            key.toStdString().c_str(),
            key.length(),
            key.toStdString().c_str(),
            NULL,
            NULL,
            &item);

    if (ret == 0) {
        ret = SecKeychainItemModifyAttributesAndData(
                item,
                NULL,
                value.length(),
                value.toStdString().c_str());

        if(ret!=0)
        {
            impl->setError(static_cast<int>(KeyManagerErrorCodes::UNABLE_TO_SAVE), "Unable to set existing value for key.");
            return false;
        }
        return true;
    } else {
        ret = SecKeychainAddGenericPassword(
                NULL,
                key.length(),
                key.toStdString().c_str(),
                key.length(),
                key.toStdString().c_str(),
                value.length(),
                value.toStdString().c_str(),
                NULL);

        if(ret!=0)
        {
            impl->setError(static_cast<int>(KeyManagerErrorCodes::UNABLE_TO_ADD),  "Unable to ret new key and value.");
            return false;
        }
        return true;
    }
}

KeyManagerError KeyManager::lastError()
{
    return impl->currentError();
}



