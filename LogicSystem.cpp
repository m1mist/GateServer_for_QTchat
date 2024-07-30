#include "LogicSystem.h"

LogicSystem::LogicSystem() {
	RegisterGet("/get test", [](std::shared_ptr<HttpConnection> connection) {
		beast::ostream(connection->response_.body()) << "receive get test request";
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

void LogicSystem::RegisterGet(std::string url, HttpHandler handler) {
	get_handlers_.insert(make_pair(url, handler));
}
