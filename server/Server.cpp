#include "./Server.hpp"
#include "./ServerSocket.hpp"
#include "./Config.hpp"
#include "../color.hpp"
#include "../error/error.hpp"
#include "../user/User.hpp"
#include "../parser/Parser.hpp"
#include "../execute/Execute.hpp"
#include "../message/Message.hpp"

ssize_t	sendNonBlocking(int fd, const char* buffer, \
		size_t dataSize) {
	ssize_t sendMsgSize = 0;

	while (1) {
		sendMsgSize = send(fd, buffer, dataSize, MSG_DONTWAIT);

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

/*
 * Helper functions
 */
static void	handleClientDisconnect(int* fd) {
	std::cout << "Client socket " << *fd \
		<< " disconnected." << std::endl;
	close(*fd);
	*fd = -1;
}

static ssize_t	recvNonBlocking(int* fd, char* buffer, \
		size_t bufferSize) {
	ssize_t	recvMsgSize = 0;

	while (1) {
		recvMsgSize = recv(*fd, buffer, bufferSize, MSG_DONTWAIT);

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

/*
 * Server class
 */
Server::Server(unsigned short port) : \
	socket_(port), config_() {
	// TODO(hnoguchi): Add try-catch
	this->fds_[0].fd = this->socket_.getFd();
	for (int i = 1; i <= this->config_.getMaxClients(); i++) {
		this->fds_[i].fd = -1;
	}
	this->fds_[this->config_.getMaxClients()].fd = STDIN_FILENO;
	for (int i = 0; i <= this->config_.getMaxClients(); i++) {
		this->fds_[i].events = POLLIN;
		this->fds_[i].revents = 0;
	}
}

Server::~Server() {}

void	Server::run() {
	while (1) {
		int result = poll(fds_, this->config_.getMaxClients() + 1, 3 * 1000);

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
	// TODO(hnoguchi): Add try-catch
	int newSocket = this->socket_.createClientSocket();
	for (int i = 1; i <= this->config_.getMaxClients(); ++i) {
		if (this->fds_[i].fd == -1) {
			this->fds_[i].fd = newSocket;
			if (static_cast<const int>(this->users_.size()) >= this->config_.getMaxClients()) {
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
			std::string	replyMsg;
			replyMsg += message.createMessage(1, user);
			replyMsg += message.createMessage(2, user);
			replyMsg += message.createMessage(3, user);
			replyMsg += message.createMessage(4, user);
			// send
			ssize_t	sendMsgSize = sendNonBlocking(this->fds_[i].fd, replyMsg.c_str(), replyMsg.size());
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
	if (this->fds_[this->config_.getMaxClients()].revents & POLLIN) {
		std::string	input;
		std::getline(std::cin, input);
		if (input == "exit") {
			std::cout << "See You..." << std::endl;
			exit(0);
		}
	}
}

void	Server::handleClientSocket() {
	for (int i = 1; i <= this->config_.getMaxClients(); ++i) {
		if (this->fds_[i].fd != -1 && (this->fds_[i].revents & POLLIN)) {
			handleReceivedData(i);
		}
	}
}

void	Server::handleReceivedData(int clientIndex) {
	char	buffer[513] = {0};
	ssize_t	sendMsgSize = 0;
	ssize_t	recvMsgSize = 0;

	recvMsgSize = recvNonBlocking(&this->fds_[clientIndex].fd, buffer, sizeof(buffer) - 1);
	if (recvMsgSize <= 0) {
		handleClientDisconnect(&this->fds_[clientIndex].fd);
		return;
	}
	// std::cout << "Client socket " << this->fds_[clientIndex].fd << " message: " << buffer << std::endl;
	std::cout << GREEN << buffer << END << std::flush;
	// Split message
	std::vector<std::string>	messages = split(buffer, "\r\n");
	Message						message;
	std::string					replyMsg("");
	for (std::vector<std::string>::iterator it = messages.begin(); \
			it != messages.end(); ++it) {
		// parse
		// TODO(hnoguchi): commandは、すべてアルファベットであれば、すべて大文字に変換すること。
		Parser	parser(*it);
		parser.tokenize();
		parser.printTokens();
		parser.parse();
		parser.getCommand().printCommand();
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<< std::endl;
		// execute
		Execute		execute(parser.getCommand());
		// std::cout << clientIndex << std::endl;
		// this->users_[clientIndex - 1].printData();
		int			replyNum = execute.exec(&this->users_[clientIndex - 1], &this->users_, &this->channels_);
		if (replyNum == 0) {
			continue;
		}
		// create replies message
		// TODO(hnoguchi): Server::getUserByFd();を実装した方が良い？
		replyMsg += message.createMessage(replyNum, this->users_[clientIndex - 1]);
		std::cout << "replyMsg: [" << replyMsg << "]" << std::endl;
	}
	if (replyMsg.empty()) {
		return;
	}
	// send
	sendMsgSize = sendNonBlocking(this->fds_[clientIndex].fd, replyMsg.c_str(), replyMsg.size());
	if (sendMsgSize <= 0) {
		handleClientDisconnect(&this->fds_[clientIndex].fd);
		return;
	}
	// TODO(hnoguchi): castは使わない実装にする？？
	if (static_cast<ssize_t>(replyMsg.size()) != sendMsgSize) {
		fatalError("send");
	}
}

// TODO(hnoguchi): エラー処理は、try-catch{}で実装する。
// TODO(hnoguchi): サーバーの終了処理を実装する。
// TODO(hnoguchi): Server classにpasswordを追加する。
// TODO(hnoguchi): <port>, <password>のバリデーションを実装する。
int	main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
	}

	(void)argv;
	Server	Server(8080);

	Server.run();
	return (0);
}
