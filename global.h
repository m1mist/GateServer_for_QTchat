#pragma once
#include "singleton.h"

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <memory>
#include <iostream>
#include <functional>
#include <map>
#include <unordered_map>
#include <queue>

#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

#include <hiredis/hiredis.h>

//¼ò»¯ÃüÃû¿Õ¼ä
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
};

constexpr auto code_prefix = "code_";
