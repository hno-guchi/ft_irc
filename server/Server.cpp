#include "./Server.hpp"
#include "../color.hpp"
#include "../user/User.hpp"
#include "../parser/Parser.hpp"
#include "../execute/Execute.hpp"
#include "../message/Message.hpp"

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

Server::Server(unsigned short port) : \
	socketFd_(0), socketAddressLen_(sizeof(socketAddress_)), maxClients_(5) {
		initializeServerSocket(port);
		initializeClientSockets();
}

void	Server::initializeServerSocket(unsigned short port) {
	this->socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (this->socketFd_ < 0) {
		fatalError("socket");
	}

	setFdFlags(this->socketFd_, O_NONBLOCK);

	memset(&this->socketAddress_, 0, sizeof(this->socketAddress_));
	this->socketAddress_.sin_family = AF_INET;
	this->socketAddress_.sin_addr.s_addr = INADDR_ANY;
	this->socketAddress_.sin_port = htons(port);

	if (bind(this->socketFd_, reinterpret_cast<struct sockaddr*>(&this->socketAddress_), \
				sizeof(this->socketAddress_)) < 0) {
		fatalError("bind");
	}
	if (listen(this->socketFd_, 3) < 0) {
		fatalError("listen");
	}
	std::cout << "Server is listening on port " << port << "..." << std::endl;
}

void	Server::initializeClientSockets() {
	this->fds_[0].fd = this->socketFd_;
	for (int i = 1; i <= this->maxClients_; ++i) {
		this->fds_[i].fd = -1;
	}
	this->fds_[this->maxClients_].fd = STDIN_FILENO;
	for (int i = 0; i <= this->maxClients_; ++i) {
		this->fds_[i].events = POLLIN;
		this->fds_[i].revents = 0;
	}
}

Server::~Server() {
	close(socketFd_);
}

void	Server::run() {
	while (1) {
		int result = poll(fds_, maxClients_ + 1, 3 * 1000);

		if (result == -1) {
			fatalError("poll");
		}
		if (result == 0) {
			// std::cout << "poll: Timeout 3 seconds..." << std::endl;
			continue;
		}
		handleServerSocket();
		handleStandardInput();
		handleClientSocket();
	}
}

void	Server::handleServerSocket() {
	if (!(this->fds_[0].revents & POLLIN)) {
		return;
	}
	int newSocket = accept(this->socketFd_, \
			reinterpret_cast<struct sockaddr*>(&this->socketAddress_), &this->socketAddressLen_);
	if (newSocket < 0) {
		close(newSocket);
		fatalError("accept");
	}
	setFdFlags(newSocket, O_NONBLOCK);
	for (int i = 1; i <= this->maxClients_; ++i) {
		if (this->fds_[i].fd == -1) {
			this->fds_[i].fd = newSocket;
			if (static_cast<const int>(this->users_.size()) >= this->maxClients_) {
				handleClientDisconnect(&this->fds_[i].fd);
				std::cerr << "Max clients reached." << std::endl;
				return;
			}
			// TODO(hnoguchi): ユーザ登録処理を追加する。
			User	user;
			user.setFd(newSocket);
			std::string nickName = "user";
			nickName += '0' + i;
			user.setNickName(nickName);
			user.printData();
			this->users_.push_back(user);
			// TODO(hnoguchi): Welcome messageはここで送信する。
			std::cout << "New client connected. Socket: " \
				<< newSocket << std::endl;
			Message		message;
			// std::string	replyMsg;
			std::string	replyMsg = ":";
			replyMsg += user.getNickName();
			replyMsg += " ";
			replyMsg += message.createMessage(1);

			replyMsg += ":";
			replyMsg += user.getNickName();
			replyMsg += " ";
			replyMsg += message.createMessage(2);

			replyMsg += ":";
			replyMsg += user.getNickName();
			replyMsg += " ";
			replyMsg += message.createMessage(3);

			replyMsg += ":";
			replyMsg += user.getNickName();
			replyMsg += " ";
			replyMsg += message.createMessage(4);
			// send
			ssize_t	sendMsgSize = sendNonBlocking(i, replyMsg.c_str(), replyMsg.size());
			if (sendMsgSize <= 0) {
				handleClientDisconnect(&this->fds_[i].fd);
				return;
			}
			// TODO(hnoguchi): castは使わない実装にする？？
			if (static_cast<ssize_t>(replyMsg.size()) != sendMsgSize) {
				// TODO(hnoguchi): Check error handling.
				fatalError("send");
			}
			break;
		}
	}
}

void	Server::handleStandardInput() {
	if (this->fds_[this->maxClients_].revents & POLLIN) {
		std::string	input;
		std::getline(std::cin, input);
		if (input == "exit") {
			std::cout << "See You..." << std::endl;
			exit(0);
		}
	}
}

void	Server::handleClientSocket() {
	for (int i = 1; i <= this->maxClients_; ++i) {
		if (this->fds_[i].fd != -1 && (this->fds_[i].revents & POLLIN)) {
			handleReceivedData(i);
		}
	}
}

static std::vector<std::string>	split(const std::string& message, \
		const std::string delim) {
	std::vector<std::string>	messages;
	std::string::size_type		startPos(0);

	while (startPos < message.size()) {
		std::string::size_type	delimPos = message.find(delim, startPos);
		if (delimPos == message.npos) {
			break;
		}
		std::string	buf = message.substr(startPos, delimPos - startPos);
		messages.push_back(buf);
		startPos = delimPos + delim.size();
	}
	return (messages);
}

void	Server::handleReceivedData(int clientIndex) {
	char	buffer[513] = {0};
	ssize_t	sendMsgSize = 0;
	ssize_t	recvMsgSize = 0;

	recvMsgSize = recvNonBlocking(clientIndex, buffer, sizeof(buffer) - 1);
	if (recvMsgSize <= 0) {
		handleClientDisconnect(&this->fds_[clientIndex].fd);
		return;
	}
	// std::cout << "Client socket " << fds_[clientIndex].fd << " message: " << buffer << std::endl;
	std::cout << GREEN << buffer << END << std::flush;
	// Split message
	std::vector<std::string>	messages = split(buffer, "\r\n");
	std::string					replyMsg;
	Message						message;
	for (std::vector<std::string>::iterator it = messages.begin(); \
			it != messages.end(); ++it) {
		// parse
		Parser	parser(*it);
		parser.tokenize();
		parser.printTokens();
		parser.parse();
		parser.getCommand().printCommand();
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<< std::endl;
		// execute
		Execute	execute(parser.getCommand());
		int		replyNum = execute.exec();
		// create replies message
		replyMsg += message.createMessage(replyNum);
	}
	// send
	sendMsgSize = sendNonBlocking(clientIndex, replyMsg.c_str(), replyMsg.size());
	if (sendMsgSize <= 0) {
		handleClientDisconnect(&this->fds_[clientIndex].fd);
		return;
	}
	// TODO(hnoguchi): castは使わない実装にする？？
	if (static_cast<ssize_t>(replyMsg.size()) != sendMsgSize) {
		fatalError("send");
	}
}

ssize_t Server::recvNonBlocking(int clientIndex, char* buffer, \
		size_t bufferSize) {
	ssize_t	recvMsgSize = 0;

	while (1) {
		recvMsgSize = recv(this->fds_[clientIndex].fd, buffer, \
				bufferSize, MSG_DONTWAIT);

		if (recvMsgSize >= 0) {
			break;
		}
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
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
	return (recvMsgSize);
}

ssize_t	Server::sendNonBlocking(int clientIndex, const char* buffer, \
		size_t dataSize) {
	ssize_t sendMsgSize = 0;

	while (1) {
		sendMsgSize = send(this->fds_[clientIndex].fd, buffer, \
				dataSize, MSG_DONTWAIT);

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

void	Server::handleClientDisconnect(int* fd) {
	std::cout << "Client socket " << *fd \
		<< " disconnected." << std::endl;
	close(*fd);
	*fd = -1;
}

int	main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
	}

	(void)argv;
	Server	Server(8080);

	Server.run();
	return (0);
}
