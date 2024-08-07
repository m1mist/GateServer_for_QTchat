#pragma once
#include <queue>
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "global.h"


using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;

class RPCConnectPool {
public:
	RPCConnectPool(size_t pool_size, const std::string& host, const std::string& port);
	~RPCConnectPool();
	std::unique_ptr<VerifyService::Stub> GetConnection();
	void Recycle(std::unique_ptr<VerifyService::Stub>);//回收连接
	void Close();

private:
	std::atomic<bool> b_stop_;
	size_t pool_size_;
	std::string host_;
	std::string port_;
	std::queue<std::unique_ptr<VerifyService::Stub>> connections_;
	std::mutex mutex_;
	std::condition_variable c_v_;
};

class VerifyGrpcClient:public Singleton<VerifyGrpcClient>{
	
public:
	friend class Singleton<VerifyGrpcClient>;
	GetVerifyRsp GetVerifyCode(std::string email) const;

private:
	VerifyGrpcClient();
	std::unique_ptr<RPCConnectPool> pool_;

};

