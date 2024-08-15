#pragma once
#include "global.h"
#include "MysqlDAO.h"
class MysqlManager : public Singleton<MysqlManager> {
	friend class Singleton<MysqlManager>;
public:
	~MysqlManager() = default;
	int RegUser(const std::string& name, const std::string& email, const std::string& passwd);
private:
	MysqlManager() = default;
	MysqlDAO  dao_;
};