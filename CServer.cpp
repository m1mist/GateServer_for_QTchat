#include "CServer.h"
#include "HttpConnection.h"
CServer::CServer(net::io_context& io_context, const unsigned short& port):
	acceptor_(io_context, tcp::endpoint(tcp::v4(),port)), io_context_(io_context), socket_(io_context){

}

void CServer::Start(){
	auto self = shared_from_this();
	acceptor_.async_accept(socket_, [self](beast::error_code err_code) {
		try{
			//出错则放弃此链接，继续监听其他链接
			if (err_code) {
				self->Start();
				return;
			}
			//创建新链接，并管理，用HttpConnect类
			std::make_shared<HttpConnection>(std::move(self->socket_))->Start();

			//继续监听
			self->Start();
		}
		catch (const std::exception& ex){
			std::cout << "exception is " << ex.what() << '\n';
		}
	});
}
