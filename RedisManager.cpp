#include "RedisManager.h"
#include "Config.h"


RedisManager::~RedisManager() {  }

bool RedisManager::Connect(const std::string& host, int port) {
	this->connect_ = redisConnect(host.c_str(), port);
	if (this->connect_ == nullptr) {
		return false;
	}
	if (this->connect_ != nullptr && this->connect_->err) {
		std::cout << "connect error" << this->connect_->errstr << '\n';
		return false;
	}
	return true;
}

bool RedisManager::Get(const std::string& key, std::string& value) {
	auto connection = pool_->GetConnection();
	if (connection == nullptr) {
		return false;
	}
	auto reply = static_cast<redisReply*>(redisCommand(connection, "GET %s", key.c_str()));
	if (reply == nullptr) {
		std::cout << "[ GET  " << key << " ] failed" << '\n';
		freeReplyObject(reply);
		pool_->Recycle(connection);
		return false;
	}

	if (reply->type != REDIS_REPLY_STRING) {
		std::cout << "[ GET  " << key << " ] failed" << '\n';
		freeReplyObject(reply);
		pool_->Recycle(connection);
		return false;
	}

	value = reply->str;
	freeReplyObject(reply);
	pool_->Recycle(connection);
	std::cout << "[ GET " << key << "  ] succeeded" << '\n';
	return true;
}

bool RedisManager::Set(const std::string& key, const std::string& value) {
	auto connection = pool_->GetConnection();
	if (connection == nullptr) {
		return false;
	}
	//执行redis命令行
	auto reply = static_cast<redisReply*>(redisCommand(connection, "SET %s %s", key.c_str(), value.c_str()));

	//如果返回NULL则说明执行失败
	if (nullptr == reply) {
		std::cout << "[ SET " << key << "  " << value << " ] failed ! " << '\n';
		freeReplyObject(reply);
		return false;
	}

	//如果执行失败则释放连接
	if (!(reply->type == REDIS_REPLY_STATUS && (strcmp(reply->str, "OK") == 0 || strcmp(reply->str, "ok") == 0))) {
		std::cout << "[ SET " << key << "  " << value << " ] failed ! " << '\n';
		freeReplyObject(reply);
		return false;
	}

	//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
	freeReplyObject(reply);
	std::cout << "[ SET " << key << "  " << value << " ] succeeded ! " << '\n';
	return true;
}

bool RedisManager::Auth(const std::string& password) {
	auto connection = pool_->GetConnection();
	if (connection == nullptr) {
		return false;
	}
	auto reply = static_cast<redisReply*>(redisCommand(connection, "AUTH %s", password.c_str()));
	if (reply->type == REDIS_REPLY_ERROR) {
		std::cout << "认证失败" << '\n';
		//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
		freeReplyObject(reply);
		return false;
	}
	else {
		//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
		freeReplyObject(reply);
		std::cout << "认证成功" << '\n';
		return true;
	}
}

bool RedisManager::LPush(const std::string& key, const std::string& value) {
		auto connection = pool_->GetConnection();
		if (connection == nullptr) {
			return false;
		}
		auto reply = static_cast<redisReply*>(redisCommand(connection, "LPUSH %s %s", key.c_str(), value.c_str()));
		if (nullptr == reply) {
			std::cout << "[ LPUSH " << key << "  " << value << " ] failed ! " << '\n';
			freeReplyObject(reply);
			return false;
		}

		if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
			std::cout << "[ LPUSH " << key << "  " << value << " ] failed ! " << '\n';
			freeReplyObject(reply);
			return false;
		}

		std::cout << "[ LPUSH " << key << "  " << value << " ] succeeded ! " << '\n';
		freeReplyObject(reply);
		return true;
	}

bool RedisManager::LPop(const std::string& key, std::string& value) {
	auto connection = pool_->GetConnection();
	if (connection == nullptr) {
		return false;
	}
	auto reply = static_cast<redisReply*>(redisCommand(connection, "LPOP %s ", key.c_str()));
	if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
		std::cout << "[ LPOP " << key << " ] failed ! " << '\n';
		freeReplyObject(reply);
		return false;
	}
	value = reply->str;
	std::cout << "[ LPOP " << key << " ] succeeded ! " << '\n';
	freeReplyObject(reply);
	return true;
}

bool RedisManager::RPush(const std::string& key, const std::string& value) {
	auto connection = pool_->GetConnection();
	if (connection == nullptr) {
		return false;
	}
	auto reply = static_cast<redisReply*>(redisCommand(connection, "RPUSH %s %s", key.c_str(), value.c_str()));
	if (nullptr == reply) {
		std::cout << "[ RPUSH " << key << "  " << value << " ] failed ! " << '\n';
		freeReplyObject(reply);
		return false;
	}

	if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
		std::cout << "[ RPUSH " << key << "  " << value << " ] failed ! " << '\n';
		freeReplyObject(reply);
		return false;
	}

	std::cout << "[ RPUSH " << key << "  " << value << " ] succeeded ! " << '\n';
	freeReplyObject(reply);
	return true;
}

bool RedisManager::RPop(const std::string& key, std::string& value) {
	auto connection = pool_->GetConnection();
	if (connection == nullptr) {
		return false;
	}
	auto reply = static_cast<redisReply*>(redisCommand(connection, "RPOP %s ", key.c_str()));
	if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
		std::cout << "[ RPOP " << key << " ] failed ! " << '\n';
		freeReplyObject(reply);
		return false;
	}
	value = reply->str;
	std::cout << "[ RPOP " << key << " ] succeeded ! " << '\n';
	freeReplyObject(reply);
	return true;
}

bool RedisManager::HSet(const std::string& key, const std::string& field, const std::string& value) {
	auto connection = pool_->GetConnection();
	if (connection == nullptr) {
		return false;
	}
	auto reply = static_cast<redisReply*>(redisCommand(connection, "HSET %s %s %s", key.c_str(), field.c_str(), value.c_str()));
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "[ HSet " << key << "  " << field << "  " << value << " ] failed ! " << '\n';
		freeReplyObject(reply);
		return false;
	}
	std::cout << "[ HSet " << key << "  " << field << "  " << value << " ] succeeded ! " << '\n';
	freeReplyObject(reply);
	return true;
}

bool RedisManager::HSet(const char* key, const char* field, const char* value, size_t value_len) {
	const char* argv[4];
	size_t argv_len[4];
	argv[0] = "HSET";
	argv_len[0] = 4;
	argv[1] = key;
	argv_len[1] = strlen(key);
	argv[2] = field;
	argv_len[2] = strlen(field);
	argv[3] = value;
	argv_len[3] = value_len;
	auto connection = pool_->GetConnection();
	if (connection == nullptr) {
		return false;
	}
	auto reply = static_cast<redisReply*>(redisCommandArgv(connection, 4, argv, argv_len));
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "[ HSet " << key << "  " << field << "  " << value << " ] failed ! " << '\n';
		freeReplyObject(reply);
		return false;
	}
	std::cout << "[ HSet " << key << "  " << field << "  " << value << " ] succeeded ! " << '\n';
	freeReplyObject(reply);
	return true;
}

std::string RedisManager::HGet(const std::string& key, const std::string& field) {
	const char* argv[3];
	size_t argv_len[3];
	argv[0] = "HGET";
	argv_len[0] = 4;
	argv[1] = key.c_str();
	argv_len[1] = key.length();
	argv[2] = field.c_str();
	argv_len[2] = field.length();
	auto connection = pool_->GetConnection();
	if (connection == nullptr) {
		return nullptr;
	}
	auto reply = static_cast<redisReply*>(redisCommandArgv(connection, 3, argv, argv_len));
	if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
		freeReplyObject(reply);
		std::cout << "[ HGet " << key << " " << field << "  ] failed ! " << '\n';
		return "";
	}

	std::string value = reply->str;
	freeReplyObject(reply);
	std::cout << "[ HGet " << key << " " << field << " ] succeeded ! " << '\n';
	return value;
}

bool RedisManager::Del(const std::string& key) {
	auto connection = pool_->GetConnection();
	if (connection == nullptr) {
		return false;
	}
	auto reply = static_cast<redisReply*>(redisCommand(connection, "DEL %s", key.c_str()));
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "[ Del " << key << " ] failed ! " << '\n';
		freeReplyObject(reply);
		return false;
	}
	std::cout << "[ Del " << key << " ] succeeded ! " << '\n';
	freeReplyObject(reply);
	return true;
}

bool RedisManager::Exists(const std::string& key) {
	auto connection = pool_->GetConnection();
	if (connection == nullptr) {
		return false;
	}
	auto reply = static_cast<redisReply*>(redisCommand(connection, "exists %s", key.c_str()));
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer == 0) {
		std::cout << "Not Found [ Key " << key << " ]  ! " << '\n';
		freeReplyObject(reply);
		return false;
	}
	std::cout << " Found [ Key " << key << " ] ! " << '\n';
	freeReplyObject(reply);
	return true;
}

void RedisManager::Close() { pool_->Close(); }

RedisManager::RedisManager(): connect_(nullptr), reply_(nullptr) {
	auto& gCfgMgr = Config::Instance();
	auto host = gCfgMgr["Redis"]["Host"];
	auto port = gCfgMgr["Redis"]["Port"];
	auto pwd = gCfgMgr["Redis"]["Passwd"];
	pool_.reset(new RedisConnectPool(5, host.c_str(), atoi(port.c_str()), pwd.c_str()));
}
