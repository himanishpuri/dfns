#include <iostream>
#include <thread>
#include <chrono>
#include "redis.h"
#include "logger.h" // Include the Logger header


RedisUtil::RedisUtil(std::string hostname, int port, int timeout) {
   this->hostname = hostname;
   this->port = port;
   this->timeout = timeout;
   this->context = nullptr;
   this->reply = nullptr;

   if (timeout == -1) {
      LOG_INFO("RedisUtil object created with hostname (%s) and port (%d)", hostname.c_str(), port);
      this->connect();
   }
   else {
      LOG_INFO("RedisUtil object created with hostname (%s), port (%d), and timeout (%d)", hostname.c_str(), port, timeout);
      this->connectWithTimeout();
      std::thread([this]() {
         std::this_thread::sleep_for(std::chrono::milliseconds(this->timeout));
         this->resetVariables();
         LOG_INFO("Redis connection timed out");
                  }).detach();
   }
}

void RedisUtil::connect() {
   this->context = redisConnect(this->hostname.c_str(), this->port);
   this->handleError(this->context, nullptr);

   LOG_INFO("Connected to Redis");
}

void RedisUtil::connectWithTimeout() {
   struct timeval timeout;
   timeout.tv_sec = this->timeout / 1000;
   timeout.tv_usec = (this->timeout % 1000) * 1000;

   this->context = redisConnectWithTimeout(this->hostname.c_str(), this->port, timeout);
   this->handleError(this->context, nullptr);

   LOG_INFO("Connected to Redis with Timeout");
}

void RedisUtil::disconnect() {
   if (this->context != nullptr) {
      redisFree(this->context);
      this->context = nullptr;
   }

   if (this->reply != nullptr) {
      freeReplyObject(this->reply);
      this->reply = nullptr;
   }

   LOG_INFO("Disconnected from Redis");
}

void RedisUtil::handleError(redisContext* context, redisReply* reply) {
   if (context == nullptr || context->err) {
      if (context) {
         std::string errMsg = "Connection error: " + std::string(context->errstr);
         LOG_ERROR(errMsg.c_str());
         this->disconnect();
      }
      else {
         LOG_ERROR("Connection error: Can't allocate redis context");
      }
      exit(1);
   }

   if (reply != nullptr) {
      if (reply->type == REDIS_REPLY_ERROR) {
         std::string errMsg = "Error: " + std::string(reply->str);
         LOG_ERROR(errMsg.c_str());
         this->disconnect();
         exit(1);
      }
   }
}

void RedisUtil::setKey(std::string key, std::string value) {
   this->reply = reinterpret_cast<redisReply*>(redisCommand(this->context, "SET %s %s", key.c_str(), value.c_str()));
   this->handleError(this->context, this->reply);
   freeReplyObject(this->reply);
   this->reply = nullptr;
   LOG_INFO("Key (%s) set to value (%s)", key.c_str(), value.c_str());
}

std::string RedisUtil::getKey(std::string key) {
   this->reply = reinterpret_cast<redisReply*>(redisCommand(this->context, "GET %s", key.c_str()));
   this->handleError(this->context, this->reply);

   std::string value = "";
   if (this->reply->type == REDIS_REPLY_NIL) {
      LOG_INFO("Key (%s) not found", key.c_str());
   }
   else {
      value = this->reply->str;
      LOG_INFO("Key (%s) has value (%s)", key.c_str(), value.c_str());
   }

   freeReplyObject(this->reply);
   this->reply = nullptr;
   return value;
}

void RedisUtil::deleteKey(std::string key) {
   this->reply = reinterpret_cast<redisReply*>(redisCommand(this->context, "DEL %s", key.c_str()));
   this->handleError(this->context, this->reply);

   if (this->reply->integer == 0) {
      LOG_WARN("Key (%s) not found", key.c_str());
   }
   else {
      LOG_INFO("Key (%s) deleted", key.c_str());
   }

   freeReplyObject(this->reply);
   this->reply = nullptr;
}

RedisUtil::~RedisUtil() {

}

void RedisUtil::resetVariables() {
   this->context = nullptr;
   this->reply = nullptr;
}