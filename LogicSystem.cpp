#include "LogicSystem.h"
#include <utility>
#include "HttpConnection.h"
#include "RedisManager.h"
#include "VerifyGrpcClient.h"
LogicSystem::LogicSystem() {
	RegisterGet("/get_test", [](const std::shared_ptr<HttpConnection>& connection) {
		beast::ostream(connection->response_.body()) << "receive get_test request\n";
		int i = 0;
		for (auto& [fst, snd] : connection->get_params_) {
			i++;
			beast::ostream(connection->response_.body()) << "param " << i << ": key is " << fst <<", "
																  << "value is " << snd << " " << '\n';
		}
	});
	/*
	 *获取验证码
	 */
	RegisterPost("/verify_code", [](const std::shared_ptr<HttpConnection>& connection) {
		auto body_str = beast::buffers_to_string(connection->request_.body().data());
		std::cout << "receive body is " << body_str << '\n';
		connection->response_.set(http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value source_root;
		if (!reader.parse(body_str, source_root)) {
			std::cout << "Failed to parse JSON data" << '\n';
			root["error"] = ErrorCodes::ERROR_JSON;
			std::string json_str = root.toStyledString();
			beast::ostream(connection->response_.body()) << json_str;
			return true;
		}
		if (!source_root.isMember("email")) {
			std::cout << "Failed to parse JSON data" << '\n';
			root["error"] = ErrorCodes::ERROR_JSON;
			std::string json_str = root.toStyledString();
			beast::ostream(connection->response_.body()) << json_str;
			return true;
		}

		auto email = source_root["email"].asString();
		GetVerifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVerifyCode(email);
		std::cout << "email is " << email << '\n';
		root["error"] = rsp.error();
		root["email"] = source_root["email"];
		std::string jsonstr = root.toStyledString();
		beast::ostream(connection->response_.body()) << jsonstr;
		return true;
	    });
	/*
	 *用户注册
	 */
	RegisterPost("/user_register", [](const std::shared_ptr<HttpConnection>& connection) {
		auto body_str = beast::buffers_to_string(connection->request_.body().data());
		std::cout << "receive body is " << body_str << '\n';
		connection->response_.set(http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value source_root;
		if (!reader.parse(body_str, source_root)) {
			std::cout << "Failed to parse JSON data" << '\n';
			root["error"] = ErrorCodes::ERROR_JSON;
			std::string json_str = root.toStyledString();
			beast::ostream(connection->response_.body()) << json_str;
			return true;
		}

		std::string verify_code;
		//查找redis中email对应的验证码是否到期
		bool b_get_verify = RedisManager::GetInstance()->Get(code_prefix+source_root["email"].asString(), verify_code);
		if(!b_get_verify) {
			std::cout << " get verify code expired" << '\n';
			root["error"] = ErrorCodes::VERIFY_EXPIRED;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->response_.body()) << jsonstr;
			return true;
		}

		//验证码是否正确
		if (verify_code != source_root["verifycode"].asString()) {
			std::cout << " verify code error" << '\n';
			root["error"] = ErrorCodes::VERIFY_CODE_ERROR;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->response_.body()) << jsonstr;
			return true;
		}

		//查找redis中是否已存在email对应的用户
		if (RedisManager::GetInstance()->Exists(source_root["email"].asString())) {
			std::cout << " user exist" << '\n';
			root["error"] = ErrorCodes::USER_EXISTS;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->response_.body()) << jsonstr;
			return true;
		}
		root["error"] = 0;
		root["email"] = source_root["email"];
		root["user"] = source_root["user"].asString();
		root["passwd"] = source_root["passwd"].asString();
		root["confirm"] = source_root["confirm"].asString();
		root["verifycode"] = source_root["verifycode"].asString();
		std::string jsonstr = root.toStyledString();
		beast::ostream(connection->response_.body()) << jsonstr;
		return true;
		});

}

LogicSystem::~LogicSystem() = default;

bool LogicSystem::HandleGet(const std::string& url, std::shared_ptr<HttpConnection> connection) {
	if (get_handlers_.find(url) == get_handlers_.end()) {
		return false;
	}
	get_handlers_[url](std::move(connection));
	return true;
}

bool LogicSystem::HandlePost(const std::string& url, std::shared_ptr<HttpConnection> connection) {
	if (post_handlers_.find(url) == post_handlers_.end()) {
		return false;
	}
	post_handlers_[url](std::move(connection));
	return true;
}

void LogicSystem::RegisterGet(const std::string& url, const HttpHandler& handler) {
	get_handlers_.insert({ url, handler });
}
void LogicSystem::RegisterPost(const std::string& url, const HttpHandler& handler) {
	post_handlers_.insert({ url, handler });
}