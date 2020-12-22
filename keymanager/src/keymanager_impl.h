#ifndef KOAD_IO_KEYMANAGER_IMPL_H
#define KOAD_IO_KEYMANAGER_IMPL_H

#include <QString>
#include <keymanager/keymanager.h>

class KeyManager::KeyManagerImpl
{
public:
    KeyManagerImpl(const QString& AppId): m_applicationId(AppId),m_lastError(nullptr)
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

    const QString& getApplicationId() const
    {
        return m_applicationId;
    }

    QString generateKeyStorageKey(const QString& key)
    {
        return QString("%1-%2").arg(m_applicationId, key);
    }

private:
    QString m_applicationId;
    KeyManagerError *m_lastError;
};

#endif
