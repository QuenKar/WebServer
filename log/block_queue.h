#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
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

        _capacity = capacity;
        _array = new T[capacity];
        _size = 0;
        _front = -1;
        _back = -1;
    }

    ~block_queue()
    {
        _mutex.lock();
        if (_array != NULL)
        {
            delete[] _array;
        }
        _mutex.unlock();
    }

    void clear()
    {
        _mutex.lock();
        _size = 0;
        _front = -1;
        _back = -1;
        _mutex.unlock();
    }

    bool isFull()
    {
        _mutex.lock();
        if (_size >= _capacity)
        {
            _mutex.unlock();
            return true;
        }
        _mutex.unlock();
        return false;
    }

    bool isEmpty()
    {
        _mutex.lock();
        if (_size == 0)
        {
            _mutex.unlock();
            return true;
        }
        _mutex.unlock();
        return false;
    }

    bool front(T &value)
    {
        _mutex.lock();
        if (_size == 0)
        {
            _mutex.unlock();
            return false;
        }
        value = _array[_front];
        _mutex.unlock();
        return true;
    }

    bool back(T &value)
    {
        _mutex.lock();
        if (_size == 0)
        {
            _mutex.unlock();
            return false;
        }
        value = _array[_back];
        _mutex.unlock();
        return true;
    }

    int size()
    {
        int s = 0;
        _mutex.lock();
        s = _size;
        _mutex.unlock();
        return s;
    }

    int capacity()
    {
        int c = 0;
        _mutex.lock();
        c = _capacity;
        _mutex.unlock();
        return c;
    }

    bool push(const T &item)
    {
        _mutex.lock();
        if (_size >= _capacity)
        {
            _cond.broadcast();
            _mutex.unlock();
            return false;
        }

        _back = (_back + 1) % _capacity;
        _array[_back] = item;
        _size++;

        _cond.broadcast();
        _mutex.unlock();
        return true;
    }

    bool pop(T &item)
    {
        _mutex.lock();
        while (_size <= 0)
        {
            if (!_cond.wait(_mutex.get()))
            {
                _mutex.unlock();
                return false;
            }
        }

        _front = (_front + 1) % _capacity;
        item = _array[_front];
        _size--;
        _mutex.unlock();
        return true;
    }

    bool pop(T &item, int ms_timeout)
    {
        struct timespec t = {0, 0};
        struct timeval now = {0, 0};

        gettimeofday(&now, NULL);
        _mutex.lock();
        if (_size <= 0)
        {
            t.tv_sec = now.tv_sec + ms_timeout / 1000;
            t.tv_nsec = ((ms_timeout % 1000) + now.tv_usec) * 1000;
            if (!_cond.timewait(_mutex, get(), t))
            {
                _mutex.unlock();
                return false;
            }
        }

        if (_size <= 0)
        {
            _mutex.unlock();
            return false;
        }

        _front = (_front + 1) % _capacity;
        item = _array[_front];
        _size--;
        _mutex.unlock();
        return true;
    }

private:
    locker _mutex;
    cond _cond;
    T *_array;
    int _size;
    int _front;
    int _back;

    int _capacity;
};

#endif