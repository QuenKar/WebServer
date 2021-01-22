#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "../lock/locker.h"
#include "../mysql_conn/sql_conn_pool.h"

template <typename T>
class threadpool
{
public:
    threadpool(int actor_model, conn_pool *connPool, int thread_number = 8, int max_request = 10000)
    {
        if (thread_number <= 0 || max_request <= 0)
        {
            throw std::exception();
        }

        m_threads = new pthread_t[m_thread_number];
        if (!m_threads)
        {
            throw std::exception();
        }

        for (int i = 0; i < thread_number; i++)
        {
            if (pthread_create(m_threads + i, NULL, worker, this) != 0)
            {
                delete[] m_threads;
                throw std::exception();
            }
            if (pthread_detach(m_threads[i]))
            {
                delete[] m_threads;
                throw new std::exception();
            }
        }
    }
    ~threadpool()
    {
        delete[] m_threads;
    }

    bool aapend(T *request, int state)
    {
        m_queuelocker.lock();
        if (m_workqueue.size() >= m_max_requests)
        {
            m_queuelocker.unlock();
            return false;
        }
        request->m_state = state;
        m_workqueue.push_back(request);
        m_queuelocker.unlock();
        m_queuestat.post();
        return true;
    }
    bool addpend_p(T *request)
    {
        m_queuelocker.lock();
        if (m_workqueue.size() >= m_max_requests)
        {
            m_queuelocker.unlock();
            return false;
        }
        m_workqueue.push_back(request);
        m_queuelocker.unlock();
        m_queuestat.post();
        return true;
    }

private:
    static void *worker(void *arg)
    {
        threadpool *pool = (threadpool *)arg;
        pool->run();
        return pool;
    }
    void run()
    {
        while (true)
        {
            m_queuestat.wait();
            m_queuelocker.lock();
            if (m_workqueue.empty())
            {
                m_queuelocker.unlock();
                continue;
            }
            T *request = m_workqueue.front();
            m_workqueue.pop_front();
            m_queuelocker.unlock();
            if (!request)
            {
                continue;
            }
            if (m_actor_model == 1)
            {
                if (request->m_state == 0)
                {
                    if (request->read_once())
                    {
                        request->improv = 1;
                        connRAII mysqlcon(&request->mysql, m_connPool);
                        request->process();
                    }
                    else
                    {
                        request->improv = 1;
                        request->timer_flag = 1;
                    }
                }
                else
                {
                    if (request->write())
                    {
                        request->improv = 1;
                    }
                    else
                    {
                        request->improv = 1;
                        request->timer_flag = 1;
                    }
                }
            }
            else
            {
                connRAII mysqlcon(&request->mysql, m_connPool);
                request->process();
            }
        }
    }

private:
    int m_thread_number;
    int m_max_requests;
    pthread_t *m_threads;
    std::list<T *> m_workqueue;
    locker m_queuelocker;
    sem m_queuestat;
    conn_pool *m_connPool;
    int m_actor_model;
};

#endif