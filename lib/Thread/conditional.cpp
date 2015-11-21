#include "conditional.h"

Conditional::Conditional()
{
    pthread_cond_init(&conditional_, NULL);
}

Conditional::~Conditional()
{
    pthread_cond_destroy(&conditional_);
}

void Conditional::wait(Mutex &mutex)
{
    pthread_cond_wait(&conditional_, mutex.getMutexPtr());
}

void Conditional::signal()
{
    pthread_cond_signal(&conditional_);
}

void Conditional::broadcast()
{
    pthread_cond_broadcast(&conditional_);
}
