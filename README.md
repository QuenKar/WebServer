# Document

:zap:c/c++实现的小型web服务器:zap:

## 功能

- 利用IO复用技术Epoll与线程池实现多线程的Reactor与Proactor高并发模型；
- 用小根堆实现定时器堆结构，实现非活动链接的超时关闭；
- 实现同步日志系统和异步日志系统（阻塞队列+单例模式+多线程）；
- 用RAII机制实现了数据库连接池；
- 用webbench进行压力测试；

## 项目结构树

```bash
WebServer
├── bin		可执行文件
├── config		配置
├── http		处理http请求响应
├── lock		RAII封装锁
├── log			日志系统
├── mysql_conn		数据库连接
├── resource		静态资源
│   └── css
├── threadpool		线程池
├── timer		定时器
├── webbench-1.5		压力测试
└── webserver		

```

## 压力测试

**测试环境：**

- 虚拟机Ubuntu 20.04
- lntel(R) Core(TM) i7-8750H CPU 4核
- 内存：8G

```
$ ./webbench -c 10000 -t 10 http://127.0.0.1:8888/

Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://127.0.0.1:8888/
10000 clients, running 10 sec.

Speed=645144 pages/min, 1204257 bytes/sec.
Requests: 107524 susceed, 0 failed.

```

可以达到10000+的QPS :smile:



## 运行

`git clone https://github.com/QuenKar/WebServer.git`

`./bin/server`

`访问：http://localhost:8888`

## 感谢

Linux高性能服务器编程，游双著