#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

class Thread
{
public:
    Thread();
    bool start(void *(*start_routine) (void *), void *arg);
    void join();
    void detach();
    void exit();
    bool isRunning() const;
protected:
    bool m_running;
    pthread_t m_thread;
};

#endif // THREAD_H
