#include "LogicSystem.h"
#include <utility>
#include "HttpConnection.h"
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
	RegisterPost("/verify_code", [](const std::shared_ptr<HttpConnection>& connection) {
		auto body_str = beast::buffers_to_string(connection->request_.body().data());
		std::cout << "receive body is " << body_str << '\n';
		connection->response_.set(http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value source_root;
		bool parse_success = reader.parse(body_str, source_root);
		if (!parse_success) {
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