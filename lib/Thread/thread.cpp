#include "thread.h"

Thread::Thread() : m_running(false)
{

}

bool Thread::start(void *(*start_routine) (void *), void *arg)
{
    if (pthread_create(&m_thread, NULL, start_routine, arg) == 0) {
        m_running = true;
        return true;
    }
    return false;
}

void Thread::join()
{
    pthread_join(m_thread, NULL);
}

void Thread::detach()
{
    pthread_detach(m_thread);
}

void Thread::exit()
{
    pthread_exit(&m_thread);
    m_running = false;
}

bool Thread::isRunning() const
{
    return m_running;
}
