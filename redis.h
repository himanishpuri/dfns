#ifndef REDIS_H
#define REDIS_H
#pragma once

#include <hiredis/hiredis.h>
#include <string>

class RedisUtil {
private:
    redisContext* context;
    redisReply* reply;
    int port; // 6379 by default
    int timeout; // (millisecond) 1.5 seconds by default
    std::string hostname; // localhost by default

public:
    RedisUtil(std::string hostname = "localhost", int port = 6379, int timeout = -1);
    ~RedisUtil();
    void handleError(redisContext* context, redisReply* reply);
    void connect();
    void setKey(std::string key, std::string value);
    std::string getKey(std::string key);
    void deleteKey(std::string key);
    void disconnect();
    void resetVariables();
    void connectWithTimeout();
};



#endif // !REDIS_H