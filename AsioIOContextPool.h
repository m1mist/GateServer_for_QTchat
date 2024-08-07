#pragma once
#include "global.h"

class AsioIOContextPool:public Singleton<AsioIOContextPool>{
public:
	friend Singleton<AsioIOContextPool>;
	using io_context = boost::asio::io_context;
	using work = boost::asio::io_context::work;

	~AsioIOContextPool();
	AsioIOContextPool(const AsioIOContextPool&) = delete;
	AsioIOContextPool& operator=(const AsioIOContextPool&) = delete;

	io_context& GetIOContext();
	void Stop();
private:
	explicit AsioIOContextPool(std::size_t size = std::thread::hardware_concurrency());
	std::vector<io_context> io_contexts_;
	std::vector<std::unique_ptr<work>> works_;
	std::vector<std::thread> threads_;
	std::size_t next_io_context_;
};

