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
//�������ռ�
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
	PASSWD_ERR =		1006,    //�������
	EMAIL_NOT_MATCH =	1007,  //���䲻ƥ��
	PASSWD_UP_FAILED =	1008,  //��������ʧ��
	PASSWD_INVALID =	1009,   //�������ʧ��
	TOKEN_INVALID =		1010,   //TokenʧЧ
	UID_INVALID =		1011,  //uid��Ч
};

constexpr auto code_prefix = "code_";

class Defer {
public:
	// ����һ��lambda���ʽ���ߺ���ָ��
	explicit Defer(const std::function<void()>& func) : func_(func) {}

	// ����������ִ�д���ĺ���
	~Defer() {
		func_();
	}

private:
	std::function<void()> func_;
};