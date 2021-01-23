#ifndef CLOSE_INACTIVE_CONN
#define CLOSE_INACTIVE_CONN

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>

#include "min_heap.h"
#include "../http/http_conn.h"

class http_conn;
class clz_conn
{
public:
    int m_TIMESLOT;
    static int *pipefd;
    time_heap timer_lst;
    static int epollfd;

public:
    clz_conn() {}
    ~clz_conn() {}

    void init(int timeslot)
    {
        m_TIMESLOT = timeslot;
    }

    int setnonblocking(int fd)
    {
        int old_option = fcntl(fd, F_GETFL);
        int new_option = old_option | O_NONBLOCK;
        fcntl(fd, F_SETFL, new_option);
        return old_option;
    }

    void addfd(int epollfd, int fd, bool one_shot, int TriggerMode)
    {
        epoll_event event;
        event.data.fd = fd;

        if (TriggerMode == 0)
            event.events = EPOLLIN | EPOLLRDHUP; //采用LT模式
        else
            event.events = EPOLLIN | EPOLLET | EPOLLRDHUP; //采用ET模式

        if (one_shot)
            event.events |= EPOLLONESHOT;
        epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
        setnonblocking(fd);
    }

    static void sig_handler(int sig)
    {
        int save_errno = errno;
        int msg = sig;
        send(pipefd[1], (char *)&msg, 1, 0);
        errno = save_errno;
    }

    void addsig(int sig, void (*handler)(int), bool restart = true)
    {
        //Structure describing the action to be taken when a signal arrives.
        struct sigaction sa;
        memset(&sa, '\0', sizeof(sa));
        sa.sa_handler = handler;

        if (restart)
            sa.sa_flags |= SA_RESTART;

        sigfillset(&sa.sa_mask);
        assert(sigaction(sig, &sa, NULL) != -1);
    }

    void timer_handler()
    {
        timer_lst.tick();
        alarm(m_TIMESLOT);
    }
};
int *clz_conn::pipefd = NULL;
int clz_conn::epollfd = 0;

void cb_func(client_data *user_data)
{
    epoll_ctl(clz_conn::epollfd, EPOLL_CTL_DEL, user_data->sockfd, 0);
    assert(user_data);
    close(user_data->sockfd);
    //改为http_conn
    http_conn::m_user_count--;
}

#endif