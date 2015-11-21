#ifndef WORKERPOOL_H
#define WORKERPOOL_H

#include <deque>
#include <vector>

#include "mutex.h"
#include "thread.h"
#include "conditional.h"

#define DEFAULT_WORKER_COUNT 4

using namespace std;

enum class PoolState {
    STOPPED, RUNNING
};

class Task
{
public:
    Task();
    Task(void (*fnPtr_)(void*), void* arg);
    ~Task();
    virtual void execute();
protected:
    bool init_;
    void* arg_;
    void (*fnPtr_)(void*);
};

class WorkerPool
{
public:
    WorkerPool();
    WorkerPool(int poolSize);
    ~WorkerPool();

    bool addTask(Task *task);
    bool addWorker();
    bool removeWorker();
    bool isRunning();

private:
    void* execute();
    friend void* doExecute(void* arg);

    int poolMaxSize_, queuedTasks_, taskLimit_;
    Mutex taskMutex_;
    Conditional tasksConditional_;
    std::vector<Thread> workerThreads_;
    std::deque<Task*> tasks_;
    PoolState poolState_;
};

#endif // WORKERPOOL_H
