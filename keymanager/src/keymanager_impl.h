#ifndef KOAD_IO_KEYMANAGER_IMPL_H
#define KOAD_IO_KEYMANAGER_IMPL_H

#include <QString>
#include <keymanager/keymanager.h>

class KeyManager::KeyManagerImpl
{
public:
    KeyManagerImpl():m_lastError(nullptr)
    {}

    ~KeyManagerImpl()
    {
        delete m_lastError;
    }

    void resetLastError()
    {
        delete m_lastError;
    }

    void setError(int code, QString message)
    {
        m_lastError = new KeyManagerError(code,message);
    }

    KeyManagerError currentError() const
    {
        if(m_lastError)
        {
            return *m_lastError;
        }
        return KeyManagerError(0,QString());
    }

private:
    KeyManagerError *m_lastError;
};

#endif
