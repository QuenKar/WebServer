#include <mysql/mysql.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <pthread.h>
#include <iostream>
#include "sql_conn_pool.h"

using namespace std;

conn_pool::conn_pool()
{
    _curConn = 0;
    _freeConn = 0;
}

conn_pool *conn_pool::getInstance()
{
    return &connPool;
}

void conn_pool::init(string url, string User, string PassWord, string DBName, int Port, int MaxConn, int close_log)
{
    _url = url;
    _port = Port;
    _user = User;
    _password = PassWord;
    _databaseName = DBName;
    _close_log = close_log;

    for (int i = 0; i < MaxConn; i++)
    {
        MYSQL *conn = NULL;

        conn = mysql_init(conn);

        if (!conn)
        {
            LOG_ERROR("MYSQL ERROR");
            exit(1);
        }
        conn = mysql_real_connect(conn, url.c_str(), User.c_str(), PassWord.c_str(), DBName.c_str(), Port, NULL, 0);

        if (conn == NULL)
        {
            LOG_ERROR("MYSQL ERROR");
            exit(1);
        }
        connList.push_back(conn);
        ++_freeConn;
    }

    reserve = sem(_freeConn);
    _maxConn = _freeConn;
}

//使用信号量reserve解决获取释放连接的同步问题
MYSQL *conn_pool::getConnection()
{
    MYSQL *conn = NULL;

    if (connList.empty())
        return NULL;

    reserve.wait();
    lock.lock();

    conn = connList.front();
    connList.pop_front();

    --_freeConn;
    ++_curConn;

    lock.unlock();
    return conn;
}

bool conn_pool::releaseConnection(MYSQL *conn)
{
    if (!conn)
        return false;

    lock.lock();

    connList.push_back(conn);
    ++_freeConn;
    --_curConn;

    lock.unlock();

    reserve.post();
    return true;
}

void conn_pool::DestroyPool()
{
    lock.lock();
    if (!connList.empty())
    {
        list<MYSQL *>::iterator it;
        for (it = connList.begin(); it != connList.end(); it++)
        {
            MYSQL *con = *it;
            mysql_close(con);
        }
        _curConn = 0;
        _freeConn = 0;
        connList.clear();
    }

    lock.unlock();
}

int conn_pool::getFreeConn()
{
    return this->_freeConn;
}

conn_pool::~conn_pool()
{
    this->DestroyPool();
}

connRAII::connRAII(MYSQL **SQL, conn_pool *connPool)
{
    *SQL = connPool->getConnection();

    cRAII = *SQL;
    poolRAII = connPool;
}

connRAII::~connRAII()
{
    poolRAII->releaseConnection(cRAII);
}
