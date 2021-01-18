#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

class sem
{
public:
    sem()
    {
        /*
        pshared参数0表示可以被一个进程的其他线程共享
        如果为非0,则可以在进程之间共享
        value指定信号量的初始值
        return：0表示成功，-1表示失败并且设置error
        */
        if (sem_init(&m_sem, 0, 0) != 0)
        {
            throw std::exception();
        }
    }
    sem(int num)
    {
        if (sem_init(&m_sem, 0, num) != 0)
        {
            throw std::exception();
        }
    }
    ~sem()
    {
        if (sem_destroy(&m_sem) != 0)
        {
            throw std::exception();
        }
    }

    bool wait()
    {
        //阻塞版信号量--
        return sem_wait(&m_sem) == 0;
    }

    bool post()
    {
        //信号量++
        return sem_post(&m_sem) == 0;
    }

private:
    sem_t m_sem;
};

class locker
{
public:
    locker()
    {
        if (pthread_mutex_init(&m_mutex, nullptr) != 0)
        {
            throw std::exception();
        }
    }
    ~locker()
    {
        if (pthread_mutex_destroy(&m_mutex) != 0)
        {
            throw std::exception();
        }
    }

    bool lock()
    {
        return pthread_mutex_lock(&m_mutex) == 0;
    }

    bool unlock()
    {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }

    pthread_mutex_t *get()
    {
        return &m_mutex;
    }

private:
    pthread_mutex_t m_mutex;
};

class cond
{
public:
    cond()
    {
        if (pthread_cond_init(&m_cond, nullptr) != 0)
        {
            throw std::exception();
        }
    }
    ~cond()
    {
        if (pthread_cond_destroy(&m_cond) != 0)
        {
            throw std::exception();
        }
    }

    bool wait(pthread_mutex_t *m_mutex)
    {
        return pthread_cond_wait(&m_cond, m_mutex) == 0;
    }

    bool timewait(pthread_mutex_t *m_mutex, struct timespec t)
    {
        /*
        If cond  has  not been signaled within the amount of time specified by ab‐
        stime, the mutex mutex is re-acquired  and  pthread_cond_timedwait  re‐
        turns the error ETIMEDOUT.
        */
        return pthread_cond_timedwait(&m_cond, m_mutex, &t) == 0;
    }

    bool signal()
    {
        return pthread_cond_signal(&m_cond) == 0;
    }

    bool broadcast()
    {
        return pthread_cond_broadcast(&m_cond) == 0;
    }

private:
    pthread_cond_t m_cond;
};

#endif