#ifndef STL_BLOCK_QUEUE_H
#define STL_BLOCK_QUEUE_H

#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <deque>
#include "../lock/locker.h"
using namespace std;

template <typename T>
class block_queue
{
public:
    block_queue(int capacity = 1000)
    {
        if (capacity <= 0)
        {
            exit(-1);
        }

        dq.resize(capacity);
    }

    ~block_queue()
    {
        _mutex.lock();
        dq.~deque();
        _mutex.unlock();
    }

    void clear()
    {
        _mutex.lock();
        dq.clear();
        _mutex.unlock();
    }

    bool isEmpty()
    {
        _mutex.lock();
        bool flag = dq.empty();
        _mutex.unlock();
        return flag;
    }

    bool front(T &value)
    {
        _mutex.lock();
        if (dq.empty())
        {
            _mutex.unlock();
            return false;
        }
        value = dq.front();
        _mutex.unlock();
        return true;
    }

    bool back(T &value)
    {
        _mutex.lock();
        if (dq.empty())
        {
            _mutex.unlock();
            return false;
        }
        value = dq.back();
        _mutex.unlock();
        return true;
    }

    int size()
    {
        int s = 0;
        _mutex.lock();
        s = dq.size();
        _mutex.unlock();
        return s;
    }

    bool push(const T &item)
    {
        _mutex.lock();

        dq.push_back(item);

        _cond.broadcast();
        _mutex.unlock();
        return true;
    }

    bool pop(T &item)
    {
        _mutex.lock();
        while (dq.size() <= 0)
        {
            if (!_cond.wait(_mutex.get()))
            {
                _mutex.unlock();
                return false;
            }
        }
        item = dq.front();
        dq.pop_front();
        _mutex.unlock();
        return true;
    }

    bool pop(T &item, int ms_timeout)
    {
        struct timespec t = {0, 0};
        struct timeval now = {0, 0};

        gettimeofday(&now, nullptr);
        _mutex.lock();
        if (dq.size() <= 0)
        {
            t.tv_sec = now.tv_sec + ms_timeout / 1000;
            t.tv_nsec = (ms_timeout % 1000) * 1000;
            if (!_cond.timewait(_mutex.get(), t))
            {
                _mutex.unlock();
                return false;
            }
        }

        if (dq.size() <= 0)
        {
            _mutex.unlock();
            return false;
        }

        item = dq.front();
        dq.pop_front();
        _mutex.unlock();
        return true;
    }

private:
    locker _mutex;
    cond _cond;

    deque<T> dq;
};

#endif