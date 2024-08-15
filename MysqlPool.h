#pragma once
#include "global.h"


class SqlConnection{
public:
	SqlConnection(sql::Connection* con, int64_t last_time):connection(con), last_operate_time(last_time) {}
	std::unique_ptr<sql::Connection> connection;
	int64_t last_operate_time;
};

class MysqlPool {
public:
	MysqlPool(std::string url, std::string user, std::string pass, std::string schema, int pool_size);

	std::unique_ptr<SqlConnection> GetConnection();		  //获得连接
	void Recycle(std::unique_ptr<SqlConnection> connection);//回收连接
	void Close();
	void CheckConnection();
	~MysqlPool();
private:
	std::string url_;
	std::string user_;
	std::string pass_;
	std::string schema_;
	int pool_size_;
	std::queue<std::unique_ptr<SqlConnection>> pool_;
	std::mutex mutex_;
	std::condition_variable c_v_;
	std::atomic<bool> b_stop_;
	std::thread check_;
};

