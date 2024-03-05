#include "./Server.hpp"
#include "../color.hpp"
#include "../parser/Parser.hpp"
#include "../execute/Execute.hpp"
#include "../reply/Reply.hpp"

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
		// TODO(hnoguchi): Message class??
		initializeReplyMessageList();
		initializeErrReplyMessageList();
}

void	Server::initializeServerSocket(unsigned short port) {
	socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFd_ < 0) {
		fatalError("socket");
	}

	setFdFlags(socketFd_, O_NONBLOCK);

	memset(&socketAddress_, 0, sizeof(socketAddress_));
	socketAddress_.sin_family = AF_INET;
	socketAddress_.sin_addr.s_addr = INADDR_ANY;
	socketAddress_.sin_port = htons(port);

	if (bind(socketFd_, reinterpret_cast<struct sockaddr*>(&socketAddress_), \
				sizeof(socketAddress_)) < 0) {
		fatalError("bind");
	}
	if (listen(socketFd_, 3) < 0) {
		fatalError("listen");
	}
	std::cout << "Server is listening on port " << port << "..." << std::endl;
}

void	Server::initializeClientSockets() {
	fds_[0].fd = socketFd_;
	for (int i = 1; i <= maxClients_; ++i) {
		fds_[i].fd = -1;
	}
	fds_[maxClients_].fd = STDIN_FILENO;
	for (int i = 0; i <= maxClients_; ++i) {
		fds_[i].events = POLLIN;
		fds_[i].revents = 0;
	}
}

void	Server::initializeReplyMessageList() {
	Reply	r;
	r.setNumeric("001");
	r.setMessage("Welcome to the Internet Relay Network <nick>!<user>@<host>");
	this->replyMessageList_.insert(std::make_pair(kRPL_WELCOME, r));

	r.setNumeric("002");
	r.setMessage("Your host is <servername>, running version <ver>");
	this->replyMessageList_.insert(std::make_pair(kRPL_YOURHOST, r));

	r.setNumeric("003");
	r.setMessage("This server was created <date>");
	this->replyMessageList_.insert(std::make_pair(kRPL_CREATED, r));

	r.setNumeric("004");
	r.setMessage("<servername> <version> <available user modes> <available channel modes>");
	this->replyMessageList_.insert(std::make_pair(kRPL_MYINFO, r));

	r.setNumeric("332");
	r.setMessage("<channel> :<topic>");
	this->replyMessageList_.insert(std::make_pair(kRPL_TOPIC, r));

	return;
}

void	Server::initializeErrReplyMessageList() {
	Reply	r;
	r.setNumeric("421");
	r.setMessage("<command> :Unknown command");
	this->errReplyMessageList_.insert(std::make_pair(kERR_UNKNOWNCOMMAND, r));

	r.setNumeric("461");
	r.setMessage("<command> :Not enough parameters");
	this->errReplyMessageList_.insert(std::make_pair(kERR_NEEDMOREPARAMS, r));

	return;
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
		for (int i = 1; i <= maxClients_; ++i) {
			handleClientSocket(i);
		}
	}
}

void	Server::handleServerSocket() {
	if (!(fds_[0].revents & POLLIN)) {
		return;
	}
	int newSocket = accept(socketFd_, \
			reinterpret_cast<struct sockaddr*>(&socketAddress_), &socketAddressLen_);
	if (newSocket < 0) {
		close(newSocket);
		fatalError("accept");
	}
	setFdFlags(newSocket, O_NONBLOCK);
	for (int i = 1; i <= maxClients_; ++i) {
		if (fds_[i].fd == -1) {
			fds_[i].fd = newSocket;
			// TODO(hnoguchi): Welcome messageはここで送信するか？？.
			std::cout << "New client connected. Socket: " \
				<< newSocket << std::endl;
			break;
		}
	}
}

void	Server::handleStandardInput() {
	if (fds_[maxClients_].revents & POLLIN) {
		std::string	input;
		std::getline(std::cin, input);
		if (input == "exit") {
			std::cout << "See You..." << std::endl;
			exit(0);
		}
	}
}

void	Server::handleClientSocket(int clientIndex) {
	if (fds_[clientIndex].fd != -1 && (fds_[clientIndex].revents & POLLIN)) {
		handleReceivedData(clientIndex);
	}
}

void	Server::handleReceivedData(int clientIndex) {
	char	buffer[1024] = {0};
	ssize_t	sendMsgSize = 0;
	ssize_t	recvMsgSize = 0;

	recvMsgSize = recvNonBlocking(clientIndex, buffer, sizeof(buffer) - 1);
	if (recvMsgSize <= 0) {
		handleClientDisconnect(clientIndex);
		return;
	}
	// std::cout << "Client socket " << fds_[clientIndex].fd << " message: " << buffer << std::endl;
	std::cout << GREEN << buffer << END << std::endl;
	// parse
	Parser	parser(buffer);
	parser.tokenize();
	// parser.printTokens();
	parser.parse();
	parser.getCommand().printCommand();
	// execute
	// Execute	execute(parser.getCommand());
	// int		reply = execute.exec();
	// create replies message
	// Message		message(reply);
	// std::string	replyMsg = message.createMessage();
	std::string	replyMsg;
	// Command		command = parser.getCommand();
	// if (parser.getCommand().getCommand() == "CAP") {
	// 	replyMsg = this->errReplyMessageList_.at(kERR_UNKNOWNCOMMAND).getNumeric();
	// 	replyMsg += " ";
	// 	replyMsg += this->errReplyMessageList_.at(kERR_UNKNOWNCOMMAND).getMessage();
	// 	replyMsg += "\r\n";

	// 	replyMsg += this->errReplyMessageList_.at(kERR_NEEDMOREPARAMS).getNumeric();
	// 	replyMsg += " ";
	// 	replyMsg += this->errReplyMessageList_.at(kERR_NEEDMOREPARAMS).getMessage();
	// 	replyMsg += "\r\n";

	// 	replyMsg += this->replyMessageList_.at(kRPL_WELCOME).getNumeric();
	// 	replyMsg += " ";
	// 	replyMsg += this->replyMessageList_.at(kRPL_WELCOME).getMessage();
	// 	replyMsg += "\r\n";

	// 	replyMsg += this->replyMessageList_.at(kRPL_YOURHOST).getNumeric();
	// 	replyMsg += " ";
	// 	replyMsg += this->replyMessageList_.at(kRPL_YOURHOST).getMessage();
	// 	replyMsg += "\r\n";

	// 	replyMsg += this->replyMessageList_.at(kRPL_CREATED).getNumeric();
	// 	replyMsg += " ";
	// 	replyMsg += this->replyMessageList_.at(kRPL_CREATED).getMessage();
	// 	replyMsg += "\r\n";

	// 	replyMsg += this->replyMessageList_.at(kRPL_MYINFO).getNumeric();
	// 	replyMsg += " ";
	// 	replyMsg += this->replyMessageList_.at(kRPL_MYINFO).getMessage();
	// 	replyMsg += "\r\n";

	// 	recvMsgSize = replyMsg.size();
	// } else {
	// 	replyMsg = this->errReplyMessageList_.at(kERR_UNKNOWNCOMMAND).getNumeric();
	// 	replyMsg += " ";
	// 	replyMsg += this->errReplyMessageList_.at(kERR_UNKNOWNCOMMAND).getMessage();
	// 	replyMsg += "\r\n";
	// }
	// send
	sendMsgSize = sendNonBlocking(clientIndex, replyMsg.c_str(), recvMsgSize);
	if (sendMsgSize <= 0) {
		handleClientDisconnect(clientIndex);
		return;
	}
	if (recvMsgSize != sendMsgSize) {
		fatalError("send");
	}
}

ssize_t Server::recvNonBlocking(int clientIndex, char* buffer, \
		size_t bufferSize) {
	ssize_t	recvMsgSize = 0;

	while (1) {
		recvMsgSize = recv(fds_[clientIndex].fd, buffer, \
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
		sendMsgSize = send(fds_[clientIndex].fd, buffer, \
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

void	Server::handleClientDisconnect(int clientIndex) {
	std::cout << "Client socket " << fds_[clientIndex].fd \
		<< " disconnected." << std::endl;
	close(fds_[clientIndex].fd);
	fds_[clientIndex].fd = -1;
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
