#include "MysqlManager.h"
int MysqlManager::RegUser(const std::string& name, const std::string& email, const std::string& passwd) {
	return dao_.RegUser(name, email, passwd);
}

bool MysqlManager::CheckEmail(const std::string& name, const std::string& email) {
	return dao_.CheckEmail(name, email);
}

bool MysqlManager::UpdatePwd(const std::string& name, const std::string& new_pwd) {
	return dao_.UpdatePasswd(name, new_pwd);
}
