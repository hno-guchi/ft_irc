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
// static void	handleClientDisconnect(int* fd) {
// 	std::cout << "Client socket " << *fd << " disconnected." << std::endl;
// 	if (*fd < 0) {
// 		return ;
// 	}
// 	close(*fd);
// 	*fd = -1;
// }

static ssize_t	recvNonBlocking(int fd, char* buffer, size_t bufferSize) {
	ssize_t	recvMsgSize = 0;

	while (1) {
		recvMsgSize = recv(fd, buffer, bufferSize, MSG_DONTWAIT);

		if (recvMsgSize >= 0) {
			break;
		}
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			errno = 0;
			recvMsgSize = 0;
			continue;
		} else if (errno == ECONNRESET) {
			// recvMsgSize = -1;
			// handleClientDisconnect(fd);
			throw std::runtime_error("recv");
		} else {
			// handleClientDisconnect(fd);
			throw std::runtime_error("recv");
		}
	}
	return (recvMsgSize);
}

// void	recvNonBlocking(int fd, const char* buffer, size_t dataSize) {
// 	ssize_t recvMsgSize = 0;
//
// 	while (1) {
// 		recvMsgSize = recv(fd, buffer, dataSize, MSG_DONTWAIT);
//
// 		if (recvMsgSize >= 0) {
// 			break;
// 		}
// 		if (errno != EAGAIN && errno != EWOULDBLOCK) {
// 			throw std::runtime_error("recv");
// 		}
// 		std::cout << "No data sent." << std::endl;
// 		errno = 0;
// 		recvMsgSize = 0;
// 	}
// 	if (recvMsgSize == 0) {
// 		throw std::runtime_error("recv");
// 	}
// 	if (static_cast<ssize_t>(dataSize) != recvMsgSize) {
// 		throw std::runtime_error("recv");
// 	}
// }


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
			// handleClientDisconnect(fd);
			throw std::runtime_error("send");
		} else {
			// handleClientDisconnect(fd);
			throw std::runtime_error("send");
		}
	}
	return (sendMsgSize);
}

// void	sendNonBlocking(int fd, const char* buffer, size_t dataSize) {
// 	ssize_t sendMsgSize = 0;
// 
// 	while (1) {
// 		sendMsgSize = send(fd, buffer, dataSize, MSG_DONTWAIT);
// 
// 		if (sendMsgSize >= 0) {
// 			break;
// 		}
// 		if (errno != EAGAIN && errno != EWOULDBLOCK) {
// 			throw std::runtime_error("send");
// 		}
// 		std::cout << "No data sent." << std::endl;
// 		errno = 0;
// 		sendMsgSize = 0;
// 	}
// 	if (sendMsgSize == 0) {
// 		throw std::runtime_error("send");
// 	}
// 	if (static_cast<ssize_t>(dataSize) != sendMsgSize) {
// 		throw std::runtime_error("send");
// 	}
// }

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
		// this->fds_[this->info_.getConfig().getMaxClient() + 1].fd = STDIN_FILENO;
		for (int i = 0; i <= this->info_.getConfig().getMaxClient(); i++) {
			this->fds_[i].events = POLLIN;
			this->fds_[i].revents = 0;
		}
	} catch (std::exception& e) {
		throw;
	}
}

Server::~Server() {
	for (int i = 0; i <= this->info_.getConfig().getMaxClient(); i++) {
		if (this->fds_[i].fd == -1) {
			continue;
		}
		close(this->fds_[i].fd);
		this->fds_[i].fd = -1;
	}
}

void	Server::run() {
	while (1) {

		int result = poll(this->fds_, this->info_.getConfig().getMaxClient() + 1, 3 * 10000);

		if (result == -1) {
			throw std::runtime_error("poll");
		}
		if (result == 0) {
			// std::cout << "poll: Timeout 3 seconds..." << std::endl;
			// TODO(hnoguchi): PINGするならここ。
			continue;
		}
		try {
			this->handleServerSocket();
			// this->handleStandardInput();
			this->handleClientSocket();
		} catch (std::exception& e) {
			throw;
		}
		// TODO(hnoguchi): PINGするならここ。
	}
}

int	Server::setFd(int fd) {
	try {
		for (int i = 1; i <= this->info_.getConfig().getMaxClient(); ++i) {
			if (this->fds_[i].fd != -1) {
				continue;
			}
			this->fds_[i].fd = fd;
			return (i);
		}
		// TODO(hnoguchi): messageを送る。
		// sendNonBlocking(fd, "Max clients reached.", sizeof("Max clients reached."));
		std::cerr << "Max clients reached." << std::endl;
		close(fd);
		return(-1);
	} catch (std::exception& e) {
		throw;
	}
}

void	Server::handleServerSocket() {
	if (!(this->fds_[0].revents & POLLIN)) {
		return;
	}
	int		i(0);
	try {
		if ((i = this->setFd(this->socket_.createClientSocket())) == -1) {
			return;
		}
		// ユーザ仮登録
		User	user;
		user.setFd(&(this->fds_[i].fd));
		this->info_.pushBackUser(user);
		std::cout << "New client connected. Socket: " << this->fds_[i].fd << std::endl;
	} catch (std::exception& e) {
		this->info_.eraseUser(this->info_.findUser(this->fds_[i].fd));
		throw;
	}
}

// void	Server::handleStandardInput() {
// 	if (!(this->fds_[this->info_.getConfig().getMaxClient() + 1].revents & POLLIN)) {
// 		return;
// 	}
// 	try {
// 		std::string	input;
// 		std::getline(std::cin, input);
// 		// TODO(hnoguchi): Check bit.
// 		if (input != "exit") {
// 			return;
// 		}
// 		std::cout << "See You..." << std::endl;
// 		throw std::runtime_error("exit");
// 	} catch (std::exception& e) {
// 		throw;
// 	}
// }

void	Server::handleClientSocket() {
	try {
		for (int i = 1; i <= this->info_.getConfig().getMaxClient(); ++i) {
			if (this->fds_[i].fd != -1 && (this->fds_[i].revents & POLLIN)) {
				// handleReceivedData(i);
				handleReceivedData(&(*this->info_.findUser(this->fds_[i].fd)));
				// std::cout << i << std::endl;
				// this->printData();
			}
		}
	} catch (std::exception& e) {
		std::cerr << RED << e.what() << END << std::endl;
		// throw;
	}
}

void	Server::handleReceivedData(User* user) {
	try {
		char	buffer[513] = {0};

		recvNonBlocking(user->getFd(), buffer, sizeof(buffer) - 1);
		// debug
		std::cout << GREEN << buffer << END << std::flush;
		// Split message
		Execute						execute;
		Reply						reply;
		std::vector<std::string>	messages = split(buffer, reply.getDelimiter());
		for (std::vector<std::string>::iterator it = messages.begin(); it != messages.end(); ++it) {
			int			replyNum = 0;
			std::string	replyMsg("");
			Parser		parser;

			replyNum = parser.parse(*it);
			// parser.getParsedMessage().printParsedMessage();
			// std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<< std::endl;
			// this->info_.getUser(i - 1).printData();
			if (replyNum == 0) {
				// 登録ユーザか確認
				if ((user->getRegistered() & kExecAllCmd) != kExecAllCmd) {
					// ユーザ登録処理
					replyMsg = execute.registerUser(user, parser.getParsedMessage(), &this->info_);
				} else {
					// コマンド実行処理
					replyMsg = execute.exec(user, parser.getParsedMessage(), &this->info_);
					// if (this->info_.getChannels().size() != 0) {
					// 	this->info_.getChannels()[0].printData();
					// }
				}
				if (!replyMsg.empty()) {
					std::string	buf = replyMsg;
					replyMsg = Reply::rplFromName(this->info_.getConfig().getServerName());
					replyMsg += buf;
				}
			} else {
				// リプライメッセージの作成
				replyMsg = reply.createMessage(replyNum, *user, this->info_, parser.getParsedMessage());
			}
			if (replyMsg.empty()) {
				continue;
			}
			// debug
			// user->printData();
			debugPrintSendMessage("replyMsg", replyMsg);
			// send
			sendNonBlocking(user->getFd(), replyMsg.c_str(), replyMsg.size());
		}
	// } catch (std::out_of_range& e) {
	// 	throw;
	} catch (std::exception& e) {
		// TODO(hnoguchi): メッセージ受信に失敗したことをユーザに通知（メッセージを送信）する？
		this->info_.eraseUser(this->info_.findUser(user->getFd()));
		throw;
	}
}

void	Server::printData() const {
	std::cout << "[SERVER DATA]__________________" << std::endl;
	std::cout << "this->fds_[" << MAX_FD << "];" << std::endl;
	for (int i = 0; i < MAX_FD; i++) {
		std::cout << "[" << i << "].fd == " << this->fds_[i].fd << std::endl;
	}
	std::cout << "this->info_;" << std::endl;
	// this->info_.printData();
	std::cout << "_______________________________" << std::endl;
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
		fatalError(e.what());
		// std::cerr << RED << e.what() << END << std::endl;
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
