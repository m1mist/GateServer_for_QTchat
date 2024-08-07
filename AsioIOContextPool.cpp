#include "AsioIOContextPool.h"

AsioIOContextPool::~AsioIOContextPool() {
	Stop();
	std::cout << "AsioIOContextPool destruct" << '\n';
}

AsioIOContextPool::io_context& AsioIOContextPool::GetIOContext() {
	auto& io_context = io_contexts_[next_io_context_++];
	if (next_io_context_ == io_contexts_.size()) {
		next_io_context_ = 0;
	}
	return io_context;
}

void AsioIOContextPool::Stop() {
	for(auto& work:works_) {
		work->get_io_context().stop();
		work.reset();
	}
	for (auto& th:threads_) {
		th.join();
	}
}

AsioIOContextPool::AsioIOContextPool(std::size_t size):
	io_contexts_(size), works_(size), next_io_context_(0) {
	for (std::size_t i = 0;i < size;i++) {
		works_[i] = std::make_unique<work>(io_contexts_[i]);
	}

	for (auto& io_context : io_contexts_) {
		threads_.emplace_back([this, &io_context]() {
			io_context.run();
			});
	}
}
