#include "LogicSystem.h"
#include <utility>
#include "HttpConnection.h"
#include "MysqlManager.h"
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
	/**
	* 注册
	*/
	RegisterPost("/user_register", [](std::shared_ptr<HttpConnection> connection) {
		auto body_str = boost::beast::buffers_to_string(connection->request_.body().data());
		std::cout << "receive body is " << body_str << '\n';
		connection->response_.set(http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value src_root;
		bool parse_success = reader.parse(body_str, src_root);
		if (!parse_success) {
			std::cout << "Failed to parse JSON data!" << '\n';
			root["error"] = ErrorCodes::ERROR_JSON;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->response_.body()) << jsonstr;
			return true;
		}

		auto email = src_root["email"].asString();
		auto name = src_root["user"].asString();
		auto pwd = src_root["passwd"].asString();
		auto confirm = src_root["confirm"].asString();

		if (pwd != confirm) {
			std::cout << "password err " << '\n';
			root["error"] = ErrorCodes::PASSWD_ERR;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->response_.body()) << jsonstr;
			return true;
		}

		//先查找redis中email对应的验证码是否合理
		std::string  verify_code;
		bool b_get_verify = RedisManager::GetInstance()->Get(code_prefix + src_root["email"].asString(), verify_code);
		if (!b_get_verify) {
			std::cout << " get verify code expired" << '\n';
			root["error"] = ErrorCodes::VERIFY_EXPIRED;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->response_.body()) << jsonstr;
			return true;
		}

		if (verify_code != src_root["verifycode"].asString()) {
			std::cout << " verify code error" << '\n';
			root["error"] = ErrorCodes::VERIFY_CODE_ERROR;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->response_.body()) << jsonstr;
			return true;
		}

		//查找数据库判断用户是否存在
		int uid = MysqlManager::GetInstance()->RegUser(name, email, pwd);
		if (uid == 0 || uid == -1) {
			std::cout << " user or email exist" << '\n';
			root["error"] = ErrorCodes::USER_EXISTS;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->response_.body()) << jsonstr;
			return true;
		}
		root["error"] = 0;
		root["uid"] = uid;
		root["email"] = email;
		root["user"] = name;
		root["passwd"] = pwd;
		root["confirm"] = confirm;
		root["verifycode"] = src_root["verifycode"].asString();
		std::string jsonstr = root.toStyledString();
		beast::ostream(connection->response_.body()) << jsonstr;
		return true;
		});
	//重置回调
	RegisterPost("/reset_pwd", [](std::shared_ptr<HttpConnection> connection) {
    auto body_str = boost::beast::buffers_to_string(connection->request_.body().data());
    std::cout << "receive body is " << body_str << '\n';
    connection->response_.set(http::field::content_type, "text/json");
    Json::Value root;
    Json::Reader reader;
    Json::Value src_root;
    bool parse_success = reader.parse(body_str, src_root);
    if (!parse_success) {
        std::cout << "Failed to parse JSON data!" << '\n';
        root["error"] = ErrorCodes::ERROR_JSON;
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->response_.body()) << jsonstr;
        return true;
    }

    auto email = src_root["email"].asString();
    auto name = src_root["user"].asString();
    auto pwd = src_root["passwd"].asString();

    //先查找redis中email对应的验证码是否合理
    std::string  verify_code;
    bool b_get_verify = RedisManager::GetInstance()->Get(code_prefix + src_root["email"].asString(), verify_code);
    if (!b_get_verify) {
        std::cout << " get verify code expired" << '\n';
        root["error"] = ErrorCodes::VERIFY_EXPIRED;
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->response_.body()) << jsonstr;
        return true;
    }

    if (verify_code != src_root["verifycode"].asString()) {
        std::cout << " verify code error" << '\n';
        root["error"] = ErrorCodes::VERIFY_CODE_ERROR;
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->response_.body()) << jsonstr;
        return true;
    }
    //查询数据库判断用户名和邮箱是否匹配
    bool email_valid = MysqlManager::GetInstance()->CheckEmail(name, email);
    if (!email_valid) {
        std::cout << " user email not match" << '\n';
        root["error"] = ErrorCodes::EMAIL_NOT_MATCH;
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->response_.body()) << jsonstr;
        return true;
    }

    //更新密码为最新密码
    bool b_up = MysqlManager::GetInstance()->UpdatePwd(name, pwd);
    if (!b_up) {
        std::cout << " update pwd failed" << '\n';
        root["error"] = ErrorCodes::PASSWD_UP_FAILED;
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->response_.body()) << jsonstr;
        return true;
    }

    std::cout << "succeed to update password:" << pwd << '\n';
    root["error"] = 0;
    root["email"] = email;
    root["user"] = name;
    root["passwd"] = pwd;
    root["verifycode"] = src_root["verifycode"].asString();
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