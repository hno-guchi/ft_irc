#include "./ServerSocket.hpp"
#include "../error/error.hpp"

void	setFdFlags(const int fd, const int setFlags) {
	int	flags = 0;

	if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
		fatalError("fcntl");
	}
	flags |= setFlags;
	if (fcntl(fd, F_SETFL, flags) < 0) {
		fatalError("fcntl");
	}
}

ServerSocket::ServerSocket(unsigned short port) :
	fd_(-1), address_(), addressLen_(sizeof(this->address_)) {
	(void)addressLen_;
	// TODO(hnoguchi): Add try-catch
	this->fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (this->fd_ < 0) {
		fatalError("socket");
	}

	setFdFlags(this->fd_, O_NONBLOCK);

	memset(&this->address_, 0, sizeof(this->address_));
	this->address_.sin_family = AF_INET;
	this->address_.sin_addr.s_addr = INADDR_ANY;
	this->address_.sin_port = htons(port);

	if (bind(this->fd_, reinterpret_cast<struct sockaddr*>(&this->address_), \
				sizeof(this->address_)) < 0) {
		fatalError("bind");
	}
	if (listen(this->fd_, 3) < 0) {
		fatalError("listen");
	}
	std::cout << "Server is listening on port " << port << "..." << std::endl;
}

ServerSocket::~ServerSocket() {
	close(this->fd_);
}

// GETTERS
int	ServerSocket::getFd() const {
	return (this->fd_);
}

int	ServerSocket::createClientSocket() const {
	int socket = accept(this->fd_, \
			reinterpret_cast<struct sockaddr*>(\
				const_cast<struct sockaddr_in*>(&this->address_)), \
			const_cast<socklen_t*>(&this->addressLen_));
	if (socket < 0) {
		close(socket);
		fatalError("accept");
	}
	setFdFlags(socket, O_NONBLOCK);
	return (socket);
}
