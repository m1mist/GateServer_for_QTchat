#include "RedisConnectPool.h"
RedisConnectPool::RedisConnectPool(size_t pool_size, const char* host, int port, const char* pwd):
	b_stop_(false), poolSize_(pool_size), host_(host), port_(port) {
	for (size_t i = 0; i < poolSize_; ++i) {
		auto* context = redisConnect(host, port);
		if (context == nullptr || context->err != 0) {
			if (context != nullptr) {
				redisFree(context);
			}
			continue;
		}

		auto reply = static_cast<redisReply*>(redisCommand(context, "AUTH %s", pwd));
		if (reply->type == REDIS_REPLY_ERROR) {
			std::cout << "��֤ʧ��" << '\n';
			//ִ��ʧ�� �ͷ�redisCommandִ�к󷵻ص�redisReply��ռ�õ��ڴ�
			freeReplyObject(reply);
			continue;
		}

		//ִ�гɹ� �ͷ�redisCommandִ�к󷵻ص�redisReply��ռ�õ��ڴ�
		freeReplyObject(reply);
		std::cout << "��֤�ɹ�" << '\n';
		connections_.push(context);
	}

}

RedisConnectPool::~RedisConnectPool() {
	std::lock_guard<std::mutex> lock(mutex_);
	while (!connections_.empty()) {
		connections_.pop();
	}
}

redisContext* RedisConnectPool::GetConnection() {
	std::unique_lock<std::mutex> lock(mutex_);
	cond_.wait(lock, [this] {
		if (b_stop_) {
			return true;
		}
		return !connections_.empty();
	});
	//���ֹͣ��ֱ�ӷ��ؿ�ָ��
	if (b_stop_) {
		return  nullptr;
	}
	auto* context = connections_.front();
	connections_.pop();
	return context;
}

void RedisConnectPool::Recycle(redisContext* context) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (b_stop_) {
		return;
	}
	connections_.push(context);
	cond_.notify_one();
}

void RedisConnectPool::Close() {
	b_stop_ = true;
	cond_.notify_all();
}
