// ReSharper disable All
#pragma once

#include "global.h"
#include "RedisConnectPool.h"

class RedisManager :public Singleton<RedisManager>, public std::enable_shared_from_this<RedisManager> {
public:
	friend class Singleton<RedisManager>;
	~RedisManager();
	bool Connect(const std::string& host, int port);										//���Ӳ���
	bool Get(const std::string& key, std::string& value);									//��ȡkey��Ӧ��value
	bool Set(const std::string& key, const std::string& value);								//����key��value
	bool Auth(const std::string& password);													//������֤
	bool LPush(const std::string& key, const std::string& value);							//���push
	bool LPop(const std::string& key, std::string& value);									//���pop
	bool RPush(const std::string& key, const std::string& value);							//�Ҳ�push
	bool RPop(const std::string& key, std::string& value);									//�Ҳ�pop
	bool HSet(const std::string& key, const std::string& field, const std::string& value);	//
	bool HSet(const char* key, const char* field, const char* value, size_t value_len);		//
	std::string HGet(const std::string& key, const std::string& field);						//��ȡ��ϣ��ָ���ֶε�ֵ��
	bool Del(const std::string& key);
	bool Exists(const std::string& key);
	void Close();
private:
	RedisManager();
	redisContext* connect_;
	redisReply* reply_;
	std::unique_ptr<RedisConnectPool> pool_;

};



