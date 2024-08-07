#include "VerifyGrpcClient.h"

#include "Config.h"

RPCConnectPool::RPCConnectPool(size_t pool_size, const std::string& host, const std::string& port):
	pool_size_(pool_size), host_(host), port_(port){
	for (size_t i = 0;i < pool_size_; i++) {
		std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ':' + port,
														        grpc::InsecureChannelCredentials());
		connections_.push(VerifyService::NewStub(channel));
	}
}

RPCConnectPool::~RPCConnectPool() {
	std::lock_guard<std::mutex> lock(mutex_);
	Close();
	while (!connections_.empty()) {
		connections_.pop();
	}
}

std::unique_ptr<VerifyService::Stub> RPCConnectPool::GetConnection() {
	std::unique_lock<std::mutex> lock(mutex_);
	c_v_.wait(lock, [this] {
		if (b_stop_) {
			return true;
		}
		return !connections_.empty();
		});
	if (b_stop_) {
		return nullptr;
	}
	std::unique_ptr<VerifyService::Stub> connection = std::move(connections_.front());
	connections_.pop();
	return connection;
}

void RPCConnectPool::Recycle(std::unique_ptr<VerifyService::Stub> connection) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (b_stop_) {
		return;
	}
	connections_.push(std::move(connection));
	c_v_.notify_one();
}

void RPCConnectPool::Close() {
	b_stop_ = true;
	c_v_.notify_all();
}

GetVerifyRsp VerifyGrpcClient::GetVerifyCode(std::string email) const
{
	ClientContext context;
	GetVerifyRsp reply;
	GetVerifyReq request;
	request.set_email(email);
	auto stub = pool_->GetConnection();
	Status status = stub->GetVerifyCode(&context, request, &reply);

	if (status.ok()) {
		pool_->Recycle(std::move(stub));
		return reply;
	}
	else {
		pool_->Recycle(std::move(stub));
		reply.set_error(ErrorCodes::RPC_FAILED);
		return reply;
	}
}

VerifyGrpcClient::VerifyGrpcClient()
{
	auto& g_cfg = Config::Instance();
	std::string host = g_cfg["VerifyServer"]["Host"];
	std::string port = g_cfg["VerifyServer"]["Port"];
	pool_.reset(new RPCConnectPool(5,host,port));
}
