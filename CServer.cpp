#include "CServer.h"

#include "AsioIOContextPool.h"
#include "HttpConnection.h"
CServer::CServer(net::io_context& io_context, const unsigned short& port):
	acceptor_(io_context, tcp::endpoint(tcp::v4(),port)), io_context_(io_context), socket_(io_context){

}

void CServer::Start(){
	auto self = shared_from_this();
	auto& io_context = AsioIOContextPool::GetInstance()->GetIOContext();
	auto new_connection = std::make_shared<HttpConnection>(io_context);
	acceptor_.async_accept(new_connection->GetSocket(), [self, new_connection](beast::error_code err_code) {
		try{
			//��������������ӣ�����������������
			if (err_code) {
				self->Start();
				return;
			}
			//���������ӣ�������
			new_connection->Start();

			//��������
			self->Start();
		}
		catch (const std::exception& ex){
			std::cout << "exception is " << ex.what() << '\n';
			self->Start();
		}
	});
}
