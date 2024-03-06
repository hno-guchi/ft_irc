#ifndef SERVER_HPP
# define SERVER_HPP

#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cstring>
#include "../user/User.hpp"
#include "../reply/Reply.hpp"

class Server {
	private:
		int							socketFd_;
		// socklen_tはコンパイラによってエラー。intの場合もある。
		socklen_t					socketAddressLen_;
		const int					maxClients_;
		struct sockaddr_in			socketAddress_;
		// TODO(hnoguchi): 定数で配列のサイズを指定する。
		std::vector<User>			users_;
		struct pollfd				fds_[7];

		void	initializeServerSocket(unsigned short port);
		void	initializeClientSockets();

		void	handleServerSocket();
		void	handleStandardInput();
		void	handleClientSocket();

		// User&	getUserByFd(int fd);

	public:
		explicit Server(unsigned short port);
		~Server();
		void	run(void);
};

#endif  // SERVER_HPP
