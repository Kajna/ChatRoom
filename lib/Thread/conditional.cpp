#include "conditional.h"

Conditional::Conditional()
{
    pthread_cond_init(&m_conditional, NULL);
}

Conditional::~Conditional()
{
    pthread_cond_destroy(&m_conditional);
}

void Conditional::wait(Mutex &mutex)
{
    pthread_cond_wait(&m_conditional, mutex.getMutexPtr());
}

void Conditional::signal()
{
    pthread_cond_signal(&m_conditional);
}

void Conditional::broadcast()
{
    pthread_cond_broadcast(&m_conditional);
}
