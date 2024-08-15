#pragma once
#include "singleton.h"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <unordered_map>

#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>

#include <hiredis/hiredis.h>

#include <mysql/jdbc.h>
//简化命名空间
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

enum ErrorCodes {
	SUCCESS =			0,
	ERROR_JSON =		1001,
	RPC_FAILED =		1002,
	VERIFY_EXPIRED =	1003,
	VERIFY_CODE_ERROR = 1004,
	USER_EXISTS =		1005,
	PASSWD_ERR =		1006,    //密码错误
	EMAIL_NOT_MATCH =	1007,  //邮箱不匹配
	PASSWD_UP_FAILED =	1008,  //更新密码失败
	PASSWD_INVALID =	1009,   //密码更新失败
	TOKEN_INVALID =		1010,   //Token失效
	UID_INVALID =		1011,  //uid无效
};

constexpr auto code_prefix = "code_";

class Defer {
public:
	// 接受一个lambda表达式或者函数指针
	explicit Defer(const std::function<void()>& func) : func_(func) {}

	// 析构函数中执行传入的函数
	~Defer() {
		func_();
	}

private:
	std::function<void()> func_;
};