#include "HttpConnection.h"
#include "LogicSystem.h"

unsigned char ToHex(unsigned char x) {
    return  x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x) {
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else assert(0);
    return y;
}
//URL编码
std::string UrlEncode(const std::string& str) {
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        //判断是否仅有数字和字母构成
        if (isalnum(static_cast<unsigned char>(str[i])) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ') //为空字符
            strTemp += "+";
        else {
            //其他字符需要提前加%并且高四位和低四位分别转为16进制
            strTemp += '%';
            strTemp += ToHex(static_cast<unsigned char>(str[i]) >> 4);
            strTemp += ToHex(static_cast<unsigned char>(str[i]) & 0x0F);
        }
    }
    return strTemp;
}
//URL解码
std::string UrlDecode(const std::string& str) {
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        //还原+为空
        if (str[i] == '+') strTemp += ' ';
        //遇到%将后面的两个字符从16进制转为char再拼接
        else if (str[i] == '%') {
            assert(i + 2 < length);
            unsigned char high = FromHex(static_cast<unsigned char>(str[++i]));
            unsigned char low = FromHex(static_cast<unsigned char>(str[++i]));
            strTemp += high * 16 + low;
        }
        else strTemp += str[i];
    }
    return strTemp;
}


HttpConnection::HttpConnection(tcp::socket socket):socket_(std::move(socket)) {}

void HttpConnection::Start() {
	auto self = shared_from_this();
	http::async_read(socket_, buffer_, request_, [self](::beast::error_code err_code,::std::size_t bytes_transferred) {
		try {
			if (err_code) {
				std::cout << "http read error is " << err_code.what() << '\n';
				return;
			}
			boost::ignore_unused(bytes_transferred);
			self->HandleRequest();
			self->DetectTimeout();
		}
		catch (const std::exception& ex) {
			std::cout << "exception is " << ex.what() << '\n';
		}
	});
}

void HttpConnection::DetectTimeout() {
	auto self = shared_from_this();

	timeout_.async_wait([self](beast::error_code err_code) {
		if (!err_code) {
			// Close socket to cancel any outstanding operation.
			self->socket_.close(err_code);
		}
	});
}

void HttpConnection::Response() {
	auto self = shared_from_this();
	response_.content_length(response_.body().size());
	http::async_write(socket_, response_, [self](beast::error_code err_code, std::size_t) {
		self->socket_.shutdown(tcp::socket::shutdown_send, err_code);
		self->timeout_.cancel();
	});
}

void HttpConnection::HandleRequest() {
	response_.version(request_.version());//设置版本
	response_.keep_alive(false);
	if (request_.method() == http::verb::get) {
		PreParseGetParams();
		bool success = LogicSystem::GetInstance()->HandleGet(get_url_, shared_from_this());
		if (!success) {
			response_.result(http::status::not_found);
			response_.set(http::field::content_type, "text/plain");
			beast::ostream(response_.body()) << "url not found\r\n";
			Response();
		}
		response_.result(http::status::ok);
		response_.set(http::field::server, "GateServer");
		Response();
		return;
	}
	if (request_.method() == http::verb::post) {
		bool success = LogicSystem::GetInstance()->HandlePost(request_.target(), shared_from_this());
		if (!success) {
			response_.result(http::status::not_found);
			response_.set(http::field::content_type, "text/plain");
			beast::ostream(response_.body()) << "url not found\r\n";
			Response();
		}
		response_.result(http::status::ok);
		response_.set(http::field::server, "GateServer");
		Response();
		return;
	}

}

void HttpConnection::PreParseGetParams() {
	// 提取 URL  e.g：get_test?key1=value1&key2=value2
	auto url = request_.target();
	// 查找查询字符串的开始位置（即 '?' 的位置）  
	auto query_pos = url.find('?');
	if (query_pos == std::string::npos) {
		get_url_ = url;
		return;
	}

	get_url_ = url.substr(0, query_pos);
	std::string query_string = url.substr(query_pos + 1);//从"?"后一个字符开始
	std::string key;
	std::string value;
	size_t pos = 0;
	while ((pos = query_string.find('&')) != std::string::npos) {
		auto pair = query_string.substr(0, pos);
		size_t eq_pos = pair.find('=');
		if (eq_pos != std::string::npos) {
			key = UrlDecode(pair.substr(0, eq_pos)); //  UrlDecode 函数来处理URL解码  
			value = UrlDecode(pair.substr(eq_pos + 1));
			get_params_[key] = value;
		}
		query_string.erase(0, pos + 1);
	}
	// 处理最后一个参数对（如果没有 & 分隔符）  
	if (!query_string.empty()) {
		size_t eq_pos = query_string.find('=');
		if (eq_pos != std::string::npos) {
			key = UrlDecode(query_string.substr(0, eq_pos));
			value = UrlDecode(query_string.substr(eq_pos + 1));
			get_params_[key] = value;
		}
	}
}
