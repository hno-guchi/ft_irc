#include "./Server.hpp"
#include "./ServerSocket.hpp"
#include "./Config.hpp"
#include "./Info.hpp"
#include "../color.hpp"
#include "../error/error.hpp"
#include "../user/User.hpp"
#include "../parser/Parser.hpp"
#include "../execute/Execute.hpp"

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
Server::Server(unsigned short port) : socket_(port) {
	// TODO(hnoguchi): Add try-catch
	this->fds_[0].fd = this->socket_.getFd();
	for (int i = 1; i <= this->info_.getConfig().getMaxClient(); i++) {
		this->fds_[i].fd = -1;
	}
	this->fds_[this->info_.getConfig().getMaxClient() + 1].fd = STDIN_FILENO;
	for (int i = 0; i <= this->info_.getConfig().getMaxClient(); i++) {
		this->fds_[i].events = POLLIN;
		this->fds_[i].revents = 0;
	}
}

Server::~Server() {}

void	Server::run() {
	// TODO(hnoguchi): Add try-catch
	while (1) {
		int result = poll(this->fds_, this->info_.getConfig().getMaxClient() + 2, 3 * 1000);

		if (result == -1) {
			fatalError("poll");
		}
		if (result == 0) {
			// std::cout << "poll: Timeout 3 seconds..." << std::endl;
			// TODO(hnoguchi): PINGするかcheck
			continue;
		}
		this->handleServerSocket();
		this->handleStandardInput();
		this->handleClientSocket();
		// TODO(hnoguchi): PINGするかcheck
	}
}

void	Server::handleServerSocket() {
	if (!(this->fds_[0].revents & POLLIN)) {
		return;
	}
	// TODO(hnoguchi): Add try-catch
	int	newSocket = this->socket_.createClientSocket();
	int	i = 1;
	for (; i <= this->info_.getConfig().getMaxClient(); ++i) {
		if (this->fds_[i].fd == -1) {
			break;
		}
	}
	if (i > this->info_.getConfig().getMaxClient()) {
		// TODO(hnoguchi): messageを送る。
		handleClientDisconnect(&newSocket);
		std::cerr << "Max clients reached." << std::endl;
		return;
	}
	// ユーザ仮登録
	this->fds_[i].fd = newSocket;
	User	user;
	user.setFd(newSocket);
	this->info_.addUser(user);
	std::cout << "New client connected. Socket: " << newSocket << std::endl;
}

void	Server::handleStandardInput() {
	if (this->fds_[this->info_.getConfig().getMaxClient() + 1].revents & POLLIN) {
		std::string	input;
		std::getline(std::cin, input);
		if (input == "exit") {
			std::cout << "See You..." << std::endl;
			exit(0);
		}
	}
}

void	Server::handleClientSocket() {
	for (int i = 1; i <= this->info_.getConfig().getMaxClient(); ++i) {
		if (this->fds_[i].fd != -1 && (this->fds_[i].revents & POLLIN)) {
			handleReceivedData(i);
		}
	}
}

void	Server::handleReceivedData(int i) {
	char	buffer[513] = {0};
	ssize_t	sendMsgSize = 0;
	ssize_t	recvMsgSize = 0;

	recvMsgSize = recvNonBlocking(&this->fds_[i].fd, buffer, sizeof(buffer) - 1);
	if (recvMsgSize <= 0) {
		handleClientDisconnect(&this->fds_[i].fd);
		// TODO(hnoguchi): this->users_も削除する。
		return;
	}
	// std::cout << "Client socket " << this->fds_[i].fd << " message: " << buffer << std::endl;
	std::cout << GREEN << buffer << END << std::flush;
	// Split message
	std::vector<std::string>	messages = split(buffer, "\r\n");
	std::string					replyMsg("");
	Execute						execute;
	Reply						reply;
	for (std::vector<std::string>::iterator it = messages.begin(); it != messages.end(); ++it) {
		// parse
		// TODO(hnoguchi): commandは、すべてアルファベットであれば、すべて大文字に変換すること。
		// TODO(hnoguchi): parser.tokenize();は、parser.parse();の中で実行する。
		Parser	parser(*it);
		parser.tokenize();
		parser.printTokens();
		parser.parse();
		parser.getParsedMessage().printParsedMessage();
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<< std::endl;
		int	replyNum = 0;
		// std::cout << i << std::endl; this->info_.getUser(i - 1).printData();
		// 登録ユーザか確認
		if ((this->info_.getUser(i - 1).getRegistered() & kExecAllCmd) != kExecAllCmd) {
			// ユーザ登録処理
			replyNum = execute.registerUser(const_cast<User *>(&this->info_.getUser(i - 1)), parser.getParsedMessage(), &this->info_);
		} else {
			// コマンド実行処理
			replyNum = execute.exec(const_cast<User *>(&this->info_.getUser(i - 1)), \
					parser.getParsedMessage(), &this->info_);
		}
		if (replyNum == 0) {
				continue;
		}
		// リプライメッセージの作成
		replyMsg += reply.createMessage(replyNum, this->info_.getUser(i - 1), this->info_, parser.getParsedMessage());
		std::cout << "replyMsg: [" << replyMsg << "]" << std::endl;
	}
	if (replyMsg.empty()) {
		return;
	}
	// send
	sendMsgSize = sendNonBlocking(this->fds_[i].fd, replyMsg.c_str(), replyMsg.size());
	if (sendMsgSize <= 0) {
		handleClientDisconnect(&this->fds_[i].fd);
		// TODO(hnoguchi): this->users_も削除する。
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
