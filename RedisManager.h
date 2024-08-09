// ReSharper disable All
#pragma once

#include "global.h"
#include "RedisConnectPool.h"

class RedisManager :public Singleton<RedisManager>, public std::enable_shared_from_this<RedisManager> {
public:
	friend class Singleton<RedisManager>;
	~RedisManager();
	bool Connect(const std::string& host, int port);										//连接操作
	bool Get(const std::string& key, std::string& value);									//获取key对应的value
	bool Set(const std::string& key, const std::string& value);								//设置key和value
	bool Auth(const std::string& password);													//密码认证
	bool LPush(const std::string& key, const std::string& value);							//左侧push
	bool LPop(const std::string& key, std::string& value);									//左侧pop
	bool RPush(const std::string& key, const std::string& value);							//右侧push
	bool RPop(const std::string& key, std::string& value);									//右侧pop
	bool HSet(const std::string& key, const std::string& field, const std::string& value);	//
	bool HSet(const char* key, const char* field, const char* value, size_t value_len);		//
	std::string HGet(const std::string& key, const std::string& field);						//获取哈希中指定字段的值。
	bool Del(const std::string& key);
	bool Exists(const std::string& key);
	void Close();
private:
	RedisManager();
	redisContext* connect_;
	redisReply* reply_;
	std::unique_ptr<RedisConnectPool> pool_;

};



