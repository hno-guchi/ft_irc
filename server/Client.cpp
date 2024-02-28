#include "./Client.hpp"

static void fatalError(const std::string& message) {
	std::perror(message.c_str());
    exit(EXIT_FAILURE);
}

static void	setFdFlags(const int fd, const int setFlags) {
	int	flags = 0;

	if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
		fatalError("fcntl");
	}
	flags |= setFlags;
	if (fcntl(fd, F_SETFL, flags) < 0) {
		fatalError("fcntl");
	}
}


Client::Client(const std::string& serverIP, unsigned short serverPort) :
	socketFd_(0) {
	this->socketFd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (this->socketFd_ < 0) {
		fatalError("socket");
	}
	setFdFlags(this->socketFd_, O_NONBLOCK);
	memset(&this->serverSocketAddress_, 0, sizeof(this->serverSocketAddress_));
	this->serverSocketAddress_.sin_family = AF_INET;
	this->serverSocketAddress_.sin_addr.s_addr = inet_addr(serverIP.c_str());
	this->serverSocketAddress_.sin_port = htons(serverPort);
}

Client::~Client() {
	close(this->socketFd_);
}

void Client::connectToServer() {
	struct pollfd	pollFd;
	int				ret = 0;

	pollFd.fd = this->socketFd_;
	pollFd.events = POLLOUT;

	ret = connect(this->socketFd_, \
				reinterpret_cast<struct sockaddr *>(&this->serverSocketAddress_), \
				sizeof(this->serverSocketAddress_));
	if (ret == 0) {
		std::cout << "Connection established!" << std::endl;
		return;
	}
	if (ret == -1 && errno != EINPROGRESS) {
		fatalError("connect");
	}
	std::cout << "Connecting..." << std::endl;
	while (1) {
		int	pollRet = 0;

		pollRet = poll(&pollFd, 1, 3 * 1000);
		if (pollRet < 0) {
			fatalError("poll");
		}
		if (pollRet == 0) {
			std::cout << "poll: Timeout 3 seconds..." << std::endl;
			continue;
		}
		if (pollFd.revents & POLLOUT) {
			std::cout << "Connection established!" << std::endl;
			break;
		}
	}
}

ssize_t	Client::sendMessage(const std::string& message) {
	ssize_t	sendMsgSize = 0;

	while (1) {
		sendMsgSize = send(this->socketFd_, message.c_str(), \
				message.size(), MSG_DONTWAIT);

		if (sendMsgSize >= 0) {
			break;
		}
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			std::cout << "No data sent." << std::endl;
			errno = 0;
			sendMsgSize = 0;
			continue;
		} else if (errno == ECONNRESET) {
			sendMsgSize = -1;
			break;
		} else {
			fatalError("send");
		}
	}
	return (sendMsgSize);
}

ssize_t	Client::receiveMessage() {
	char	buffer[1024] = {0};
	int		recvMsgSize = 0;

	std::cout << "Received: " << std::flush;
	while (1) {
		// Non blocking
		recvMsgSize = recv(this->socketFd_, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
		if (recvMsgSize >= 0) {
			break;
		}
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			// std::cout << "No data received." << std::endl;
			errno = 0;
			recvMsgSize = 0;
			continue;
		} else if (errno == ECONNRESET) {
			recvMsgSize = -1;
			break;
		} else {
			fatalError("recv");
		}
	}
	buffer[recvMsgSize] = '\0';
	std::cout << buffer << std::endl;
	return (recvMsgSize);
}

// ssize_t	Client::receiveMessage() {
// 	size_t	totalSizeRecved = 0;
// 
// 	std::cout << "Received: " << std::flush;
// 	while (totalSizeRecved < this->messageSize_) {
// 		while (1) {
// 			char	echoBuffer[RCVBUFSIZE] = {0};
// 			int		sizeRecved = 0;
// 
// 			// Non blocking
// 			sizeRecved = recv(this->socketFd_, echoBuffer, RCVBUFSIZE - 1, MSG_DONTWAIT);
// 			if (sizeRecved < 0) {
// 				if (errno == EAGAIN || errno == EWOULDBLOCK) {
// 					// std::cout << "No data received." << std::endl;
// 					errno = 0;
// 					continue;
// 				} else {
// 					fatalError("recv");
// 				}
// 			} else {
// 				echoBuffer[sizeRecved] = '\0';
// 				std::cout << echoBuffer << std::flush;
// 				totalSizeRecved += sizeRecved;
// 				break;
// 			}
// 		}
// 	}
// 	std::cout << std::endl;
// }

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <Echo Word>\n";
		exit(1);
	}

	const std::string	message = argv[1];

	Client	client("127.0.0.1", 8080);

	client.connectToServer();
	while (1) {
		ssize_t	sendMsgSize = 0;
		ssize_t	recvMsgSize = 0;

		sendMsgSize = client.sendMessage(message);
		if (sendMsgSize < 0) {
			break;
		}
		recvMsgSize = client.receiveMessage();
		if (recvMsgSize < 0) {
			break;
		}
		if (errno == ETIMEDOUT) {
			std::cerr << errno << ": " << strerror(ETIMEDOUT) << std::endl;
			break;
		}
		if (sendMsgSize != recvMsgSize) {
			fatalError("recv");
		}
		sleep(5);
	}
	return (0);
}
