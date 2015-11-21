#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>

class Mutex
{
public:
    Mutex();
    ~Mutex();
    void lock();
    void unlock();
    bool isLocked() const;
    pthread_mutex_t* getMutexPtr();
private:
    pthread_mutex_t mutex_;
    volatile bool locked_;
};
#endif // MUTEX_H
