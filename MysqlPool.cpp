#include "MysqlPool.h"

void MysqlPool::CheckConnection() {
	std::lock_guard<std::mutex> lock(mutex_);
	int pool_size = pool_.size();
	// ��ȡ��ǰʱ���
	auto current_time = std::chrono::system_clock::now().time_since_epoch();
	// ��ʱ���ת��Ϊ��
	long long time_stamp = std::chrono::duration_cast<std::chrono::seconds>(current_time).count();
	for (int i = 0; i < pool_size; i++) {
		auto con = std::move(pool_.front());
		pool_.pop();
		Defer defer([this, &con]() {
			pool_.push(std::move(con));
			});

		if (time_stamp - con->last_operate_time < 5) {
			continue;
		}
		/*
		 *time_stamp - con->last_operate_time > 5s
		 *����
		 *��������
		 */
		try {
			//����һ�β�ѯ
			std::unique_ptr<sql::Statement> stmt(con->connection->createStatement());
			stmt->executeQuery("SELECT 1");
			con->last_operate_time = time_stamp;
			std::cout << "execute timer alive query , cur is " << time_stamp << '\n';
		}
		catch (sql::SQLException& e) {
			std::cout << "Error keeping connection alive: " << e.what() << '\n';
			// ���´������Ӳ��滻�ɵ�����
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
			auto* new_connection = driver->connect(url_, user_, pass_);
			new_connection->setSchema(schema_);
			con->connection.reset(new_connection);
			con->last_operate_time = time_stamp;
		}
	}
}
MysqlPool::MysqlPool(std::string url, std::string user, std::string pass, std::string schema, const int pool_size)
	:url_(std::move(url)), user_(std::move(user)), pass_(std::move(pass)), schema_(std::move(schema)), pool_size_(pool_size){
	try {
		for (int i = 0; i < pool_size_; ++i) {
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
			auto* con = driver->connect(url_, user_, pass_);
			con->setSchema(schema_);
			// ��ȡ��ǰʱ���
			auto current_time = std::chrono::system_clock::now().time_since_epoch();
			// ��ʱ���ת��Ϊ��
			long long time_stamp = std::chrono::duration_cast<std::chrono::seconds>(current_time).count();
			pool_.push(std::make_unique<SqlConnection>(con, time_stamp));
		}

		check_ = std::thread([this]() {
			while (!b_stop_) {
				CheckConnection();
				std::this_thread::sleep_for(std::chrono::seconds(60));
			}
			});

		check_.detach();
	}
	catch (sql::SQLException& e) {
		// �����쳣
		std::cout << "mysql pool init failed, error is " << e.what() << '\n';
	}
}

std::unique_ptr<SqlConnection> MysqlPool::GetConnection() {
	std::unique_lock<std::mutex> lock(mutex_);
	c_v_.wait(lock, [this] {
		if (b_stop_) {
			return true;
		}
		return !pool_.empty();
		});
	if (b_stop_) {
		return nullptr;
	}
	std::unique_ptr<SqlConnection> connection(std::move(pool_.front()));
	return connection;
}

void MysqlPool::Recycle(std::unique_ptr<SqlConnection> connection) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (b_stop_) {
		return;
	}
	pool_.push(std::move(connection));
	c_v_.notify_one();
}

void MysqlPool::Close() {
	b_stop_ = true;
	c_v_.notify_all();
}

MysqlPool::~MysqlPool() {
	std::unique_lock<std::mutex> lock(mutex_);
	while (!pool_.empty()) {
		pool_.pop();
	}
};
