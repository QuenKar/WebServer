CXX = g++
CFLAGS = -std=c++14 -O2 -Wall -g

TARGET = server
OBJS = main.cpp ./timer/close_inactive_conn.cpp ./http/http_conn.cpp ./log/log.cpp ./mysql_conn/sql_connection_pool.cpp ./webserver/webserver.cpp ./config/config.cpp

all: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o ./bin/$(TARGET)  -pthread -lmysqlclient

clean:
	rm -rf ./bin/$(TARGET)