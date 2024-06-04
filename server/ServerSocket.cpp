#include "./ServerSocket.hpp"
#include "../debug/debug.hpp"

// CONSTRUCTOR
ServerSocket::ServerSocket(unsigned short port) :
	fd_(-1), address_(), addressLen_(sizeof(this->address_)) {
	try {
		this->fd_ = socket(AF_INET, SOCK_STREAM, 0);
		if (this->fd_ < 0) {
			throw std::runtime_error("socket");
		}
		if (fcntl(this->fd_, F_SETFL, O_NONBLOCK) < 0) {
			throw std::runtime_error("fcntl");
		}
		memset(&this->address_, 0, sizeof(this->address_));
		this->address_.sin_family = AF_INET;
		this->address_.sin_addr.s_addr = INADDR_ANY;
		// this->address_.sin_addr.s_addr = inet_addr("224.10.10.2");
		this->address_.sin_port = htons(port);

		if (bind(this->fd_, reinterpret_cast<struct sockaddr*>(&this->address_), sizeof(this->address_)) < 0) {
			throw std::runtime_error("bind");
		}
		if (listen(this->fd_, 3) < 0) {
			throw std::runtime_error("listen");
		}
		std::cout << "Server is listening on port " << port << "..." << std::endl;
	} catch (std::exception& e) {
		if (this->fd_ >= 0) {
			close(this->fd_);
		}
		throw;
	}
}

ServerSocket::~ServerSocket() {
	close(this->fd_);
}

// GETTERS
int	ServerSocket::getFd() const {
	return (this->fd_);
}

int	ServerSocket::createClientSocket() const {
	int socket = accept(this->fd_, reinterpret_cast<struct sockaddr*>(const_cast<struct sockaddr_in*>(&this->address_)), const_cast<socklen_t*>(&this->addressLen_));
	if (socket < 0) {
		throw std::runtime_error("accept");
	}
	if (fcntl(socket, F_SETFL, O_NONBLOCK) < 0) {
		close(socket);
		throw std::runtime_error("fcntl");
	}
	return (socket);
}
