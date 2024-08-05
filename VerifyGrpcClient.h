#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "global.h"
#include "singleton.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;

class VerifyGrpcClient:public Singleton<VerifyGrpcClient>{
	friend class Singleton<VerifyGrpcClient>;
public:

	GetVerifyRsp GetVerifyCode(std::string email) {
		ClientContext context;
		GetVerifyRsp reply;
		GetVerifyReq request;
		request.set_email(email);

		Status status = stub_->GetVerifyCode(&context, request, &reply);

		if (status.ok()) {

			return reply;
		}
		else {
			reply.set_error(ErrorCodes::RPC_FAILED);
			return reply;
		}
	}

private:
	VerifyGrpcClient() {
		std::shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());
		stub_ = VerifyService::NewStub(channel);
	}

	std::unique_ptr<VerifyService::Stub> stub_;
};

