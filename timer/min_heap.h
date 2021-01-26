#ifndef MIN_HEAP
#define MIN_HEAP

#include <iostream>
#include <netinet/in.h>
#include <time.h>

using std::exception;

#define BUFFER_SIZE 64

class util_timer;

struct client_data
{
    sockaddr_in address;
    int sockfd;
    char buf[BUFFER_SIZE];
    util_timer *timer;
};

class util_timer
{
public:
    util_timer() {}

    time_t expire;
    void (*cb_func)(client_data *);
    client_data *user_data;
};

class time_heap
{
public:
    time_heap(int cap = 800, int ts = 5) : capacity(cap), cur_size(0)
    {
        timeslot = ts;
        array = new util_timer *[capacity];
        if (!array)
        {
            throw std::exception();
        }
        for (int i = 0; i < capacity; i++)
        {
            array[i] = NULL;
        }
        cur_size = 0;
    }

    time_heap(util_timer **init_array, int size, int capacity) : cur_size(size), capacity(capacity)
    {
        if (capacity < size)
        {
            throw std::exception();
        }

        array = new util_timer *[capacity];
        if (!array)
        {
            throw std::exception();
        }
        for (int i = 0; i < capacity; i++)
        {
            array[i] = NULL;
        }
        if (size != 0)
        {
            for (int i = 0; i < size; i++)
            {
                array[i] = init_array[i];
            }
            for (int i = (cur_size - 1) / 2; i >= 0; i--)
            {
                percolate_down(i);
            }
        }
    }

    ~time_heap()
    {
        for (int i = 0; i < cur_size; i++)
        {
            delete array[i];
        }
        delete[] array;
    }

public:
    void add_timer(util_timer *timer)
    {
        if (!timer)
        {
            return;
        }
        if (cur_size >= capacity)
        {
            resize();
        }
        int hole = cur_size++;
        int parent = 0;

        for (; hole > 0; hole = parent)
        {
            parent = (hole - 1) / 2;
            if (array[parent]->expire <= timer->expire)
            {
                break;
            }
            array[hole] = array[parent];
        }
        array[hole] = timer;
    }

    void del_timer(util_timer *timer)
    {
        if (!timer)
        {
            return;
        }
        //延迟销毁，节省定时器删除的时间，缺点是数组易膨胀
        timer->cb_func = NULL;
    }

    void adjust_timer(util_timer *timer)
    {
        if (!timer)
        {
            return;
        }
        time_t cur = time(NULL);
        timer->expire = cur + 3 * timeslot;
        for (int i = 0; i < cur_size; i++)
        {
            if (array[i] == timer)
            {
                percolate_down(i);
                break;
            }
        }
    }

    util_timer *top() const
    {
        if (empty())
        {
            return NULL;
        }
        return array[0];
    }

    void pop_timer()
    {
        if (empty())
        {
            return;
        }
        if (array[0])
        {
            delete array[0];

            array[0] = array[--cur_size];
            percolate_down(0);
        }
    }

    void tick()
    {
        util_timer *tmp = array[0];
        time_t cur = time(NULL);

        while (!empty())
        {
            if (!tmp)
            {
                break;
            }

            if (tmp->expire > cur)
            {
                break;
            }

            if (array[0]->cb_func)
            {
                array[0]->cb_func(array[0]->user_data);
            }

            pop_timer();
            tmp = array[0];
        }
    }

private:
    bool empty() const
    {
        return cur_size == 0;
    }

    void percolate_down(int hole)
    {
        util_timer *temp = array[hole];
        int child = 0;
        for (; ((hole * 2 + 1) <= (cur_size - 1)); hole = child)
        {
            child = hole * 2 + 1;
            if ((child < (cur_size - 1)) && (array[child + 1]->expire < array[child]->expire))
            {
                ++child;
            }
            if (array[child]->expire < temp->expire)
            {
                array[hole] = array[child];
            }
            else
            {
                break;
            }
        }
        array[hole] = temp;
    }

    void resize()
    {
        util_timer **temp = new util_timer *[2 * capacity];
        if (!temp)
        {
            throw std::exception();
        }
        for (int i = 0; i < 2 * capacity; i++)
        {
            temp[i] = NULL;
        }
        capacity *= 2;

        for (int i = 0; i < cur_size; i++)
        {
            temp[i] = array[i];
        }

        delete[] array;
        array = temp;
    }

private:
    util_timer **array;
    int capacity;
    int cur_size;
    int timeslot;
};

#endif