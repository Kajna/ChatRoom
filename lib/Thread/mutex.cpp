#include "mutex.h"

Mutex::Mutex() : m_locked(false)
{
    pthread_mutex_init(&m_mutex, NULL);
}

Mutex::~Mutex()
{
    while(m_locked);
    unlock(); // Unlock Mutex after shared resource is safe
    pthread_mutex_destroy(&m_mutex);
}

void Mutex::lock()
{
    pthread_mutex_lock(&m_mutex);
    m_locked = true;
}

void Mutex::unlock()
{
    m_locked = false; // do it BEFORE unlocking to avoid race condition
    pthread_mutex_unlock(&m_mutex);
}

pthread_mutex_t* Mutex::getMutexPtr()
{
    return &m_mutex;
}

bool Mutex::isLocked() const
{
    return m_locked;
}

