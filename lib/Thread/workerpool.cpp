#include "workerpool.h"

WorkerPool::WorkerPool() : m_pool_state(PoolState::STOPPED), m_pool_max_size(DEFAULT_WORKER_COUNT)
{

}

WorkerPool::WorkerPool(int poolSize) : m_pool_state(PoolState::STOPPED)
{
    m_pool_max_size = poolSize;
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
        m_task_mutex.lock();

        // We need to put pthread_cond_wait in a loop for two reasons:
        // 1. There can be spurious wakeups (due to signal/ENITR)
        // 2. When mutex is released for waiting, another thread can be waken up
        //    from a signal/broadcast and that thread can mess up the condition.
        //    So when the current thread wakes up the condition may no longer be
        //    actually true!
        while ((m_pool_state != PoolState::STOPPED) && (m_tasks.empty())) {
            // Wait until there is a task in the queue
            // Unlock mutex while wait, then lock it back when signaled
            m_tasks_conditional.wait(m_task_mutex);
        }

        // If the thread was woken up to notify process shutdown, return from here
        if (m_pool_state == PoolState::STOPPED) {
            m_task_mutex.unlock();
            pthread_exit(NULL);
        }

        task = m_tasks.front();
        m_tasks.pop_front();
        m_task_mutex.unlock();

        // Execute the task
        task->execute();
        delete task;
    }

    return nullptr;
}

bool WorkerPool::addTask(Task* task)
{
    m_task_mutex.lock();

    if (m_tasks.size() + 1 > m_task_limit) {
        m_task_mutex.unlock();
        return false;
    }

    m_tasks.push_back(task);

     // Wake up one thread that is waiting for a task to be available
    m_tasks_conditional.signal();

    m_task_mutex.unlock();

    return true;
}

bool WorkerPool::addWorker()
{
    if (m_pool_max_size == m_worker_threads.size()) {
        return false;
    }

    Thread new_thread;

    new_thread.start(&doExecute, this);

    m_worker_threads.push_back(new_thread);

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
