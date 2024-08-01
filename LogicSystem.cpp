#include "LogicSystem.h"
#include "HttpConnection.h"

LogicSystem::LogicSystem() {
	RegisterGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
		beast::ostream(connection->response_.body()) << "receive get_test request\n";
		int i = 0;
		for (auto& elem : connection->get_params_) {
			i++;
			beast::ostream(connection->response_.body()) << "param " << i << ": key is " << elem.first <<", "<< " value is " << elem.second << " " << std::endl;
		}
	});
	RegisterPost("/auth_code", [](std::shared_ptr<HttpConnection> connection) {
		auto body_str = beast::buffers_to_string(connection->request_.body().data());
		std::cout << "receive body is " << body_str << std::endl;
		connection->response_.set(http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value source_root;
		bool parse_success = reader.parse(body_str, source_root);
		if (!parse_success) {
			std::cout << "Failed to parse JSON data" << std::endl;
			root["error"] = ErrorCodes::ERROR_JSON;
			std::string json_str = root.toStyledString();
			beast::ostream(connection->response_.body()) << json_str;
			return true;
		}
		if (!source_root.isMember("email")) {
			std::cout << "Failed to parse JSON data" << std::endl;
			root["error"] = ErrorCodes::ERROR_JSON;
			std::string json_str = root.toStyledString();
			beast::ostream(connection->response_.body()) << json_str;
			return true;
		}

		auto email = source_root["email"].asString();
		std::cout << "email is " << email << std::endl;
		root["error"] = 0;
		root["email"] = source_root["email"];
		std::string jsonstr = root.toStyledString();
		beast::ostream(connection->response_.body()) << jsonstr;
		return true;
	});
}

LogicSystem::~LogicSystem() {
	
}

bool LogicSystem::HandleGet(std::string url, std::shared_ptr<HttpConnection> connection) {
	if (get_handlers_.find(url) == get_handlers_.end()) {
		return false;
	}
	get_handlers_[url](connection);
	return true;
}

bool LogicSystem::HandlePost(std::string url, std::shared_ptr<HttpConnection> connection) {
	if (post_handlers_.find(url) == post_handlers_.end()) {
		return false;
	}
	post_handlers_[url](connection);
	return true;
}

void LogicSystem::RegisterGet(std::string url, HttpHandler handler) {
	get_handlers_.insert(make_pair(url, handler));
}
void LogicSystem::RegisterPost(std::string url, HttpHandler handler) {
	post_handlers_.insert(make_pair(url, handler));
}