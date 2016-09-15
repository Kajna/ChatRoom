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

    int m_pool_max_size, m_queued_tasks, m_task_limit;

    Mutex m_task_mutex;
    Conditional m_tasks_conditional;

    PoolState m_pool_state;
    std::deque<Task*> m_tasks;
    std::vector<Thread> m_worker_threads;
};

#endif // WORKERPOOL_H
