#include "MysqlDAO.h"
#include "Config.h"

MysqlDAO::MysqlDAO() {
	auto& cfg = Config::Instance();
	const auto& host = cfg["Mysql"]["Host"];
	const auto& port = cfg["Mysql"]["Port"];
	const auto& pwd = cfg["Mysql"]["Passwd"];
	const auto& schema = cfg["Mysql"]["Schema"];
	const auto& user = cfg["Mysql"]["User"];
	pool_.reset(new MysqlPool(host + ":" + port, user, pwd, schema, 5));
}
MysqlDAO::~MysqlDAO() {
	pool_->Close();
}
int MysqlDAO::RegUser(const std::string& name, const std::string& email, const std::string& passwd) {
	auto connection = pool_->GetConnection();
	try {
		if (connection == nullptr) {

			return false;
		}
		// 准备调用存储过程
		std::unique_ptr < sql::PreparedStatement > stmt(connection->connection->prepareStatement("CALL reg_user(?,?,?,@result)"));
		// 设置输入参数
		stmt->setString(1, name);
		stmt->setString(2, email);
		stmt->setString(3, passwd);
		// 执行存储过程
		stmt->execute();

		std::unique_ptr<sql::Statement> stmt_result(connection->connection->createStatement());
		std::unique_ptr<sql::ResultSet> res(stmt_result->executeQuery("SELECT @result AS result"));
		if (res->next()) {
			int result = res->getInt("result");
			std::cout << "Result: " << result << '\n';
			pool_->Recycle(std::move(connection));
			return result;
		}
		pool_->Recycle(std::move(connection));
		return -1;
	}
	catch (sql::SQLException& e) {
		pool_->Recycle(std::move(connection));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << '\n';
		return -1;
	}
}

//int MysqlDAO::RegUserTransaction(const std::string & name, const std::string & email, const std::string & passwd, const std::string & icon) {
//
//}
//
bool MysqlDAO::CheckEmail(const std::string& name, const std::string& email) {
		auto con = pool_->GetConnection();
	try {
		if (con == nullptr) {
			pool_->Recycle(std::move(con));
			return false;
		}

		// 准备查询语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->connection->prepareStatement("SELECT email FROM user WHERE name = ?"));

		// 绑定参数
		pstmt->setString(1, name);

		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

		// 遍历结果集
		while (res->next()) {
			std::cout << "Check Email: " << res->getString("email") << '\n';
			if (email != res->getString("email")) {
				pool_->Recycle(std::move(con));
				return false;
			}
			pool_->Recycle(std::move(con));
			return true;
		}
	}
	catch (sql::SQLException& e) {
		pool_->Recycle(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << '\n';
		return false;
	}
}

bool MysqlDAO::UpdatePasswd(const std::string& name, const std::string& new_passwd) {
		auto con = pool_->GetConnection();
	try {
		if (con == nullptr) {
			pool_->Recycle(std::move(con));
			return false;
		}

		// 准备查询语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->connection->prepareStatement("UPDATE user SET passwd = ? WHERE name = ?"));

		// 绑定参数
		pstmt->setString(2, name);
		pstmt->setString(1, new_passwd);

		// 执行更新
		int updateCount = pstmt->executeUpdate();

		std::cout << "Updated rows: " << updateCount << '\n';
		pool_->Recycle(std::move(con));
		return true;
	}
	catch (sql::SQLException& e) {
		pool_->Recycle(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << '\n';
		return false;
	}
}

//bool MysqlDAO::CheckPasswd(const std::string& name, const std::string& pwd, UserInfo& user_info) {
//	return false;
//}
//
//bool MysqlDAO::TestProcedure(const std::string& email, int& uid, std::string& name) {
//	return false;
//}
