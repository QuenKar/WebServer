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

class clz_conn
{
public:
    clz_conn() {}
    ~clz_conn() {}

    void init(int timeslot);

    int setnonblocking(int fd);

    void addfd(int epollfd, int fd, bool one_shot, int TRIGMode);

    static void sig_handler(int sig);

    void addsig(int sig, void(handler)(int), bool restart = true);

    void timer_handler();

    void show_error(int connfd, const char *info);

public:
    static int *pipefd;
    time_heap timer_lst;
    static int epollfd;
    int m_TIMESLOT;
};

void cb_func(client_data *user_data);

#endif