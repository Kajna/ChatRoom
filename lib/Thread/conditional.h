#ifndef CONDITIONAL_H
#define CONDITIONAL_H

#include "mutex.h"

class Conditional
{
public:
    Conditional();
    ~Conditional();
    void wait(Mutex &mutex);
    void signal();
    void broadcast();
private:
    pthread_cond_t conditional_;
};

#endif // CONDITIONAL_H
