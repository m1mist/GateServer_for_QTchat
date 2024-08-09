#pragma once


#include "global.h"


class RedisConnectPool {
public:
	RedisConnectPool(size_t pool_size, const char* host, int port, const char* pwd);

	~RedisConnectPool();

	redisContext* GetConnection();

	void Recycle(redisContext* context);

	void Close();

private:
	std::atomic<bool> b_stop_;
	size_t poolSize_;
	const char* host_;
	int port_;
	std::queue<redisContext*> connections_;
	std::mutex mutex_;
	std::condition_variable cond_;
};
