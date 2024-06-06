#include "./Server.hpp"
#include "./ServerSocket.hpp"
#include "./Config.hpp"
#include "./Info.hpp"
#include "../color.hpp"
#include "../debug/debug.hpp"
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
#ifdef DEBUG
	std::cout << "FROM: " << fd << " [" << GREEN << buffer << END << "]" << std::endl;
#endif  // DEBUG
}

void	Server::sendNonBlocking(int fd, const char* buffer, size_t dataSize) {
#ifdef DEBUG
	std::cout << "TO  : " << fd << " [" << YELLOW << buffer << END << "]" << std::endl;
#endif  // DEBUG
	ssize_t	sendMsgSize = send(fd, buffer, dataSize, MSG_DONTWAIT);
	if (sendMsgSize < 0) {
		throw std::runtime_error("send");
	}
}

// CONSTRUCTOR
Server::Server(unsigned short port, const std::string& connectPwd) : socket_(port), info_(connectPwd) {
	try {
		this->fds_[0].fd = this->socket_.getFd();
		for (int i = 1; i <= this->info_.getMaxClient(); i++) {
			this->fds_[i].fd = -1;
		}
		for (int i = 0; i <= this->info_.getMaxClient(); i++) {
			this->fds_[i].events = POLLIN;
			this->fds_[i].revents = 0;
		}
	} catch (std::exception& e) {
		throw;
	}
}

Server::~Server() {
	for (int i = 0; i <= this->info_.getMaxClient(); i++) {
		if (this->fds_[i].fd == -1) {
			continue;
		}
		close(this->fds_[i].fd);
		this->fds_[i].fd = -1;
	}
}

void	Server::run() {
	while (1) {
		int result = poll(this->fds_, this->info_.getMaxClient() + 1, 3 * 10000);

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
		// TODO(hnoguchi): PINGするならここ。
	}
}

int	Server::setFd(int fd) {
	try {
		for (int i = 1; i <= this->info_.getMaxClient(); ++i) {
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
		// throw;
	}
}

void	Server::handleClientSocket() {
	try {
		for (int i = 1; i <= this->info_.getMaxClient(); ++i) {
			if (this->fds_[i].fd != -1 && (this->fds_[i].revents & POLLIN)) {
				handleReceivedData(*this->info_.findUser(this->fds_[i].fd));
#ifdef DEBUG
				this->info_.debugPrintInfo();
#endif  // DEBUG
			}
		}
	} catch (std::exception& e) {
#ifdef DEBUG
		debugPrintErrorMessage(e.what());
#endif  // DEBUG
		// throw;
	}
}

void	Server::handleReceivedData(User* user) {
	try {
		char	buffer[513] = {0};

		recvNonBlocking(user->getFd(), buffer, sizeof(buffer) - 1);
		std::string					bufferStr(buffer);
		if (user->getLeftMsg().size() != 0) {
			bufferStr = user->getLeftMsg() + bufferStr;
			user->setLeftMsg("");
		}
		std::vector<std::string>	messages = this->split(buffer, user);
		for (std::vector<std::string>::iterator it = messages.begin(); it != messages.end(); ++it) {
			Parser		parser;
			Execute		execute;

			// 登録ユーザか確認
			if ((user->getRegistered() & kExecAllCmd) != kExecAllCmd) {
				// ユーザ登録処理
				if (parser.registerParse(*user, *it, this->info_) == -1) {
					continue;
				}
				execute.registerUser(user, parser.getParsedMsg(), &this->info_);
			} else {
				// 登録済みユーザのコマンド実行処理
				if (parser.parse(*user, *it, this->info_) == -1) {
					continue;
				}
				execute.exec(user, parser.getParsedMsg(), &this->info_);
			}
		}
	} catch (std::exception& e) {
		// TODO(hnoguchi): メッセージ受信に失敗したことをユーザに通知（メッセージを送信）する？
		this->info_.eraseUser(this->info_.findUser(user->getFd()));
		throw;
	}
}

void	Server::debugPrintServer() const {
	std::cout << "[SERVER DATA]__________________" << std::endl;
	std::cout << "this->fds_[" << MAX_FD << "];" << std::endl;
	for (int i = 0; i < MAX_FD; i++) {
		std::cout << "[" << i << "].fd == " << this->fds_[i].fd << std::endl;
	}
	std::cout << "this->info_;" << std::endl;
	// this->info_.printData();
	std::cout << "_______________________________" << std::endl;
}
