#include <signal.h>
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
static void	recvNonBlocking(int fd, char* buffer, size_t dataSize) {
	ssize_t	recvMsgSize = recv(fd, buffer, dataSize, MSG_DONTWAIT);
	if (recvMsgSize < 0) {
		throw std::runtime_error("recv");
	}
}

/*
 * Global functions
 */
void	sendNonBlocking(int fd, const char* buffer, size_t dataSize) {
	ssize_t	sendMsgSize = send(fd, buffer, dataSize, MSG_DONTWAIT);
	if (sendMsgSize < 0) {
		throw std::runtime_error("send");
	}
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
			this->handleClientSocket();
		} catch (std::exception& e) {
			throw;
		}
		this->info_.printInfo();
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
		this->info_.pushBackUser(new User(&(this->fds_[i].fd)));
		std::cout << "New client connected. Socket: " << this->fds_[i].fd << std::endl;
	} catch (std::exception& e) {
		this->info_.eraseUser(this->info_.findUser(this->fds_[i].fd));
		throw;
	}
	// this->info_.printUsers();
}

void	Server::handleClientSocket() {
	try {
		for (int i = 1; i <= this->info_.getConfig().getMaxClient(); ++i) {
			if (this->fds_[i].fd != -1 && (this->fds_[i].revents & POLLIN)) {
				handleReceivedData(*this->info_.findUser(this->fds_[i].fd));
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
		Execute						execute;
		Reply						reply;
		// Split message
		std::string					bufferStr(buffer);
		if (user->getLeftMsg().size() != 0) {
			bufferStr = user->getLeftMsg() + bufferStr;
			user->setLeftMsg("");
		}
		std::vector<std::string>	messages = split(buffer, reply.getDelimiter(), user);
		for (std::vector<std::string>::iterator it = messages.begin(); it != messages.end(); ++it) {
			int			replyNum = 0;
			std::string	replyMsg("");
			// std::vector<SendMsg *>	sendList;
			Parser		parser;

			// parser.parse(*it, this->info_.getConfig().getCommandList(), &sendList);
			replyNum = parser.parse(*it, this->info_.getConfig().getCommandList());
			// parser.getParsedMsg().printParsedMsg();
			// std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<< std::endl;
			// if (sendList.size() == 0) {
			if (replyNum != -1) {
				// 登録ユーザか確認
				if ((user->getRegistered() & kExecAllCmd) != kExecAllCmd) {
					// ユーザ登録処理
					replyMsg = execute.registerUser(user, parser.getParsedMsg(), &this->info_);
				} else {
					// コマンド実行処理
					replyMsg = execute.exec(user, parser.getParsedMsg(), &this->info_);
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
				replyMsg = reply.createMessage(replyNum, *user, this->info_, parser.getParsedMsg());
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
	} catch (std::exception& e) {
		// TODO(hnoguchi): メッセージ受信に失敗したことをユーザに通知（メッセージを送信）する？
		// this->info_.eraseUser(this->info_.findUser(user->getFd()));
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
