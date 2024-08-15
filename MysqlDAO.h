#pragma once
#include "global.h"
#include "MysqlPool.h"
struct UserInfo {
	std::string name;
	std::string passwd;
	int uid;
	std::string email;
};

class MysqlDAO {
public:
	MysqlDAO();
	~MysqlDAO();
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	int RegUserTransaction(const std::string& name, const std::string& email, const std::string& passwd, const std::string& icon);
	bool CheckEmail(const std::string& name, const std::string& email);
	bool UpdatePasswd(const std::string& name, const std::string& new_passwd);
	bool CheckPasswd(const std::string& name, const std::string& pwd, UserInfo& user_info);
	bool TestProcedure(const std::string& email, int& uid, std::string& name);
private:
	std::unique_ptr<MysqlPool> pool_;
};
