#include "mutex.h"

Mutex::Mutex() : locked_(false)
{
    pthread_mutex_init(&mutex_, NULL);
}

Mutex::~Mutex()
{
    while(locked_);
    unlock(); // Unlock Mutex after shared resource is safe
    pthread_mutex_destroy(&mutex_);
}

void Mutex::lock()
{
    pthread_mutex_lock(&mutex_);
    locked_ = true;
}

void Mutex::unlock()
{
    locked_ = false; // do it BEFORE unlocking to avoid race condition
    pthread_mutex_unlock(&mutex_);
}

pthread_mutex_t* Mutex::getMutexPtr()
{
    return &mutex_;
}

bool Mutex::isLocked() const
{
    return locked_;
}

