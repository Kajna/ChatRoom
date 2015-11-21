#include "workerpool.h"

WorkerPool::WorkerPool() : poolState_(PoolState::STOPPED), poolMaxSize_(DEFAULT_WORKER_COUNT)
{

}

WorkerPool::WorkerPool(int poolSize) : poolState_(PoolState::STOPPED)
{
    poolMaxSize_ = poolSize;
}

WorkerPool::~WorkerPool()
{

}

void* doExecute(void* arg)
{
    return static_cast<WorkerPool*>(arg)->execute();
}

void* WorkerPool::execute()
{
    Task* task = 0;

    while(true) {
        taskMutex_.lock();

        // We need to put pthread_cond_wait in a loop for two reasons:
        // 1. There can be spurious wakeups (due to signal/ENITR)
        // 2. When mutex is released for waiting, another thread can be waken up
        //    from a signal/broadcast and that thread can mess up the condition.
        //    So when the current thread wakes up the condition may no longer be
        //    actually true!
        while ((poolState_ != PoolState::STOPPED) && (tasks_.empty())) {
            // Wait until there is a task in the queue
            // Unlock mutex while wait, then lock it back when signaled
            tasksConditional_.wait(taskMutex_);
        }

        // If the thread was woken up to notify process shutdown, return from here
        if (poolState_ == PoolState::STOPPED) {
            taskMutex_.unlock();
            pthread_exit(NULL);
        }

        task = tasks_.front();
        tasks_.pop_front();
        taskMutex_.unlock();

        // Execute the task
        task->execute();
        delete task;
    }

    return nullptr;
}

bool WorkerPool::addTask(Task* task)
{
    taskMutex_.lock();

    if (tasks_.size() + 1 > taskLimit_) {
        taskMutex_.unlock();
        return false;
    }

    tasks_.push_back(task);

     // Wake up one thread that is waiting for a task to be available
    tasksConditional_.signal();

    taskMutex_.unlock();

    return true;
}

bool WorkerPool::addWorker()
{
    if (poolMaxSize_ == workerThreads_.size()) {
        return false;
    }

    Thread tNew;

    tNew.start(&doExecute, this);

    workerThreads_.push_back(tNew);

    return true;
}

bool WorkerPool::removeWorker()
{

}

Task::Task() : init_(false)
{
}

Task::Task(void (*fnPtr)(void*), void* arg) : fnPtr_(fnPtr), arg_(arg), init_(true)
{
}

Task::~Task()
{
}

void Task::execute()
{
    if (init_) {
        (*fnPtr_)(arg_);
    }
}
