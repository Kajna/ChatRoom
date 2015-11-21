#include "thread.h"

Thread::Thread() : running_(false)
{

}

bool Thread::start(void *(*start_routine) (void *), void *arg)
{
    if (pthread_create(&thread_, NULL, start_routine, arg) == 0) {
        running_ = true;
        return true;
    }
    return false;
}

void Thread::join()
{
    pthread_join(thread_, NULL);
}

void Thread::detach()
{
    pthread_detach(thread_);
}

void Thread::exit()
{
    pthread_exit(&thread_);
    running_ = false;
}

bool Thread::isRunning() const
{
    return running_;
}
