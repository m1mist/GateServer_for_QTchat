#include "MysqlManager.h"
int MysqlManager::RegUser(const std::string& name, const std::string& email, const std::string& passwd) {
	return dao_.RegUser(name, email, passwd);
}
