#pragma once
#include "global.h"

class CServer:public std::enable_shared_from_this<CServer>{
public:
	CServer(net::io_context& io_context, unsigned short& port);
	void Start();
private:
	tcp::acceptor acceptor_;
	net::io_context& io_context_;
	tcp::socket socket_;
};

