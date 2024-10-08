﻿#include "CServer.h"
#include "Config.h"
#include "MysqlManager.h"
#include "RedisManager.h"


void TestMysqlMgr() {
	int id = MysqlManager::GetInstance()->RegUser("wwc","secondtonone1@163.com","123456");
	std::cout << "id  is " << id << std::endl;
}
int main() {
	//TestMysqlMgr()  ;
	MysqlManager::GetInstance();
	RedisManager::GetInstance();
    auto& g_cfg = Config::Instance();
    std::string gate_port_str = g_cfg["GateServer"]["Port"];
    unsigned short gate_port = atoi(gate_port_str.c_str());
    try {
        //设置端口
        unsigned short port = static_cast<unsigned short>(8080);

        net::io_context ioc{ 1 };

        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
            if (error) {
                return;
            }
            ioc.stop();
        });
        std::make_shared<CServer>(ioc, port)->Start();
        std::cout << "Gate Server listen on port: " << port << '\n';
        ioc.run();
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
