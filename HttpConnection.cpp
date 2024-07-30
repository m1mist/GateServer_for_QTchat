#include "HttpConnection.h"

HttpConnection::HttpConnection(tcp::socket socket):socket_(std::move(socket)) {}

void HttpConnection::Start() {
	auto self = shared_from_this();
	http::async_read(socket_, buffer_, request_, [self](::beast::error_code err_code,::std::size_t bytes_transferred) {
		try {
			if (err_code) {
				std::cout << "http read error is " << err_code.what() << std::endl;
				return;
			}
			boost::ignore_unused(bytes_transferred);
			self->HandleRequest();
			self->DetectTimeout();
		}
		catch (const std::exception& ex) {
			std::cout << "exception is " << ex.what() << std::endl;
		}
	});
}

void HttpConnection::DetectTimeout() {
	auto self = shared_from_this();

	timeout_.async_wait([self](beast::error_code err_code) {
		if (!err_code) {
			// Close socket to cancel any outstanding operation.
			self->socket_.close(err_code);
		}
	});
}

void HttpConnection::Response() {
	auto self = shared_from_this();
	response_.content_length(response_.body().size());
	http::async_write(socket_, response_, [self](beast::error_code err_code, std::size_t) {
		self->socket_.shutdown(tcp::socket::shutdown_send, err_code);
		self->timeout_.cancel();
	});
}

void HttpConnection::HandleRequest() {
	response_.version(request_.version());//ÉèÖÃ°æ±¾
	response_.keep_alive(false);
	if (request_.method() == http::verb::get) {
		bool success = LogicSystem::GetInstance()->HandleGet(request_.target(), shared_from_this());
		if (!success) {
			response_.result(http::status::not_found);
			response_.set(http::field::content_type, "text/plain");
			beast::ostream(response_.body()) << "url not found\r\n";
			Response();
		}
		response_.result(http::status::ok);
		response_.set(http::field::server, "GateServer");
		Response();
		return;
	}

}
