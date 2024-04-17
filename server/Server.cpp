#include "./Server.hpp"
#include "./ServerSocket.hpp"
#include "./Config.hpp"
#include "./Info.hpp"
#include "../color.hpp"
#include "../error/error.hpp"
#include "../user/User.hpp"
#include "../parser/Parser.hpp"
#include "../execute/Execute.hpp"

/*
 * Helper functions
 */
static void	handleClientDisconnect(int* fd) {
	std::cout << "Client socket " << *fd << " disconnected." << std::endl;
	if (*fd < 0) {
		return ;
	}
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
			// recvMsgSize = -1;
			// break;
			handleClientDisconnect(fd);
			throw std::runtime_error("recv");
		} else {
			// fatalError("recv");
			handleClientDisconnect(fd);
			throw std::runtime_error("recv");
		}
	}
	return (recvMsgSize);
}

static std::vector<std::string>	split(const std::string& message, const std::string delim) {
	try {
		std::vector<std::string>	messages;
		std::string::size_type		startPos(0);

		while (startPos < message.size()) {
			std::string::size_type	delimPos = message.find(delim, startPos);
			if (delimPos == message.npos) {
				break;
			}
			// TODO(hnoguchi): std::string::substr(); throw exception std::out_of_range();
			std::string	buf = message.substr(startPos, delimPos - startPos);
			messages.push_back(buf);
			startPos = delimPos + delim.size();
		}
		return (messages);
	} catch (std::exception& e) {
		throw;
	}
}

/*
 * Global functions
 */
ssize_t	sendNonBlocking(int fd, const char* buffer, size_t dataSize) {
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
			// sendMsgSize = -1;
			// break;
			// handleClientDisconnect(fd);
			throw std::runtime_error("send");
		} else {
			// fatalError("send");
			// handleClientDisconnect(fd);
			throw std::runtime_error("send");
		}
	}
	return (sendMsgSize);
}

/*
 * Server class
 */
// CONSTRUCTOR
Server::Server(unsigned short port) : socket_(port), info_() {
	try {
		this->fds_[0].fd = this->socket_.getFd();
		for (int i = 1; i <= this->info_.getConfig().getMaxClient(); i++) {
			this->fds_[i].fd = -1;
		}
		this->fds_[this->info_.getConfig().getMaxClient() + 1].fd = STDIN_FILENO;
		for (int i = 0; i <= this->info_.getConfig().getMaxClient(); i++) {
			this->fds_[i].events = POLLIN;
			this->fds_[i].revents = 0;
		}
	} catch (std::exception& e) {
		throw;
	}
}

Server::~Server() {
	for (int i = 1; i <= this->info_.getConfig().getMaxClient(); i++) {
		if (this->fds_[i].fd != -1) {
			close(this->fds_[i].fd);
		}
	}
}

void	Server::run() {
	while (1) {

		int result = poll(this->fds_, this->info_.getConfig().getMaxClient() + 2, 3 * 1000);

		if (result == -1) {
			// TODO(hnoguchi): fatalerrorにするか？
			throw std::runtime_error("poll");
		}
		if (result == 0) {
			// std::cout << "poll: Timeout 3 seconds..." << std::endl;
			// TODO(hnoguchi): PINGするかcheck
			continue;
		}
		this->handleServerSocket();
		try {
			this->handleStandardInput();
		} catch (std::exception& e) {
			throw;
		}
		this->handleClientSocket();
		// TODO(hnoguchi): PINGするかcheck
	}
}

void	Server::handleServerSocket() {
	if (!(this->fds_[0].revents & POLLIN)) {
		return;
	}
	int	i = 1;
	try {
		int	newSocket = this->socket_.createClientSocket();
		for (; i <= this->info_.getConfig().getMaxClient(); ++i) {
			if (this->fds_[i].fd == -1) {
				break;
			}
		}
		if (i > this->info_.getConfig().getMaxClient()) {
			// TODO(hnoguchi): messageを送る。
			std::cerr << "Max clients reached." << std::endl;
			close(newSocket);
			return;
		}
		// ユーザ仮登録
		this->fds_[i].fd = newSocket;
		User	user(i);
		user.setFd(newSocket);
		this->info_.addUser(user);
		std::cout << "New client connected. Socket: " << newSocket << std::endl;
	} catch (std::exception& e) {
		// TODO(hnoguchi): messageを送る。
		handleClientDisconnect(&this->fds_[i].fd);
		// TODO(hnoguchi): this->info_.removeUser(i - 1);
	}
}

void	Server::handleStandardInput() {
	if (!(this->fds_[this->info_.getConfig().getMaxClient() + 1].revents & POLLIN)) {
		return;
	}
	try {
		std::string	input;
		std::getline(std::cin, input);
		// TODO(hnoguchi): Check bit.
		if (input != "exit") {
			return;
		}
		std::cout << "See You..." << std::endl;
		throw std::runtime_error("exit");
	} catch (std::exception& e) {
		throw;
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
	try {
		char	buffer[513] = {0};
		ssize_t	sendMsgSize = 0;

		recvNonBlocking(&this->fds_[i].fd, buffer, sizeof(buffer) - 1);
		// ssize_t	recvMsgSize = recvMsgSize = recvNonBlocking(&this->fds_[i].fd, buffer, sizeof(buffer) - 1);
		// if (recvMsgSize <= 0) {
		// 	handleClientDisconnect(&this->fds_[i].fd);
		// 	// TODO(hnoguchi): this->users_も削除する。
		// 	return;
		// }
		// std::cout << "Client socket " << this->fds_[i].fd << " message: " << buffer << std::endl;
		std::cout << GREEN << buffer << END << std::flush;
		// Split message
		std::vector<std::string>	messages = split(buffer, "\r\n");
		std::string					replyMsg("");
		Execute						execute;
		Reply						reply;
		for (std::vector<std::string>::iterator it = messages.begin(); it != messages.end(); ++it) {
			Parser	parser;
			int		replyNum = 0;

			replyNum = parser.parse(*it);
			parser.getParsedMessage().printParsedMessage();
			std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<< std::endl;
			// std::cout << i << std::endl; this->info_.getUser(i - 1).printData();
			if (replyNum == 0) {
				// 登録ユーザか確認
				if ((this->info_.getUser(i - 1).getRegistered() & kExecAllCmd) != kExecAllCmd) {
					// ユーザ登録処理
					replyNum = execute.registerUser(const_cast<User *>(&this->info_.getUser(i - 1)), parser.getParsedMessage(), &this->info_);
				} else {
					// コマンド実行処理
					replyNum = execute.exec(const_cast<User *>(&this->info_.getUser(i - 1)), parser.getParsedMessage(), &this->info_);
				}
			}
			// if (replyNum < 0) {
			// 	return;
			// }
			this->info_.getUser(i - 1).printData();
			if (replyNum == 0) {
					continue;
			}
			// リプライメッセージの作成
			replyMsg += reply.createMessage(replyNum, this->info_.getUser(i - 1), this->info_, parser.getParsedMessage());
		}
		if (replyMsg.empty()) {
			return;
		}
		// send
		debugPrintSendMessage("replyMsg", replyMsg);
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
	} catch (std::out_of_range& e) {
		std::cerr << RED << e.what() << END << std::endl;
		return;
	} catch (std::exception& e) {
		// TODO(hnoguchi): メッセージ受信に失敗したことをユーザに通知（メッセージを送信）する？
		handleClientDisconnect(&this->fds_[i].fd);
		// TODO(hnoguchi): this->users_を削除する。
		std::cerr << RED << e.what() << END << std::endl;
	}
}

// TODO(hnoguchi): サーバーの終了処理を実装する。
// TODO(hnoguchi): Server classにpasswordを追加する。
// TODO(hnoguchi): <port>, <password>のバリデーションを実装する。
int	main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
	}

	try {
		(void)argv;
		Server	Server(8080);

		Server.run();
	} catch (std::exception& e) {
		std::cerr << RED << e.what() << END << std::endl;
		// destruct
		// return (1);
	}
	return (0);
}

// #ifdef SERVER_LEAKS
// __attribute__((destructor)) static void destructor() {
//     system("leaks -q ircserv");
// }
// #endif  // SERVER_LEAKS
