#ifndef SQL_CONN_POOL
#define SQL_CONN_POOL

#include <stdio.h>
#include <list>
#include <mysql/mysql.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <string>
#include "../lock/locker.h"
#include "../log/log.h"

using namespace std;

class conn_pool
{
public:
    MYSQL *getConnection();
    bool releaseConnection(MYSQL *conn);
    int getFreeConn();
    void DestroyPool();

    static conn_pool *getInstance();

    void init(string url, string user, string password, string databasename, int port, int maxConn, int close_log);

private:
    conn_pool();
    ~conn_pool();

    int _maxConn;
    int _curConn;
    int _freeConn;
    locker lock;
    list<MYSQL *> connList;

    //可用连接信号量
    sem reserve;
    //mysql连接池
    static conn_pool connPool;

public:
    string _url;
    string _port;
    string _user;
    string _password;
    string _databaseName;
    int _close_log;
};

class connRAII
{
public:
    connRAII(MYSQL **conn, conn_pool *connPoll);
    ~connRAII();

private:
    MYSQL *cRAII;
    conn_pool *poolRAII;
};

#endif