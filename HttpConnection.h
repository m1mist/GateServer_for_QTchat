#pragma once
#include "global.h"


class HttpConnection:public std::enable_shared_from_this<HttpConnection>{
public:
	friend class LogicSystem;
	HttpConnection(tcp::socket socket);
	void Start();
private:
	void DetectTimeout();
	void Response();
	void HandleRequest();
	tcp::socket socket_;
	beast::flat_buffer buffer_{8192};
	http::request<http::dynamic_body> request_;
	http::response<http::dynamic_body> response_;
	net::steady_timer timeout_{
		socket_.get_executor(), std::chrono::seconds(60)
	};
};

