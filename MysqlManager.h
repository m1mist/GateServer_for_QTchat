#pragma once
#include "global.h"
#include "MysqlDAO.h"
class MysqlManager : public Singleton<MysqlManager> {
	friend class Singleton<MysqlManager>;
public:
	~MysqlManager() = default;
	int RegUser(const std::string& name, const std::string& email, const std::string& passwd);
	bool CheckEmail(const std::string& name, const std::string& email);
	bool UpdatePwd(const std::string& name, const std::string& new_pwd);
private:
	MysqlManager() = default;
	MysqlDAO  dao_;
};