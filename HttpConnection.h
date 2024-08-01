#pragma once
#include "global.h"

unsigned char ToHex(unsigned char x);
unsigned char FromHex(unsigned char x);
std::string UrlEncode(const std::string& str);
std::string UrlDecode(const std::string& str);

class HttpConnection:public std::enable_shared_from_this<HttpConnection>{
public:
	friend class LogicSystem;
	HttpConnection(tcp::socket socket);
	void Start();
private:
	void DetectTimeout();	 //超时检测
	void Response();
	void HandleRequest();
	void PreParseGetParams();//解析参数
	std::string get_url_;
	std::unordered_map<std::string, std::string> get_params_;
	tcp::socket socket_;
	beast::flat_buffer buffer_{8192};
	http::request<http::dynamic_body> request_;
	http::response<http::dynamic_body> response_;
	net::steady_timer timeout_{
		socket_.get_executor(), std::chrono::seconds(60)
	};
};

