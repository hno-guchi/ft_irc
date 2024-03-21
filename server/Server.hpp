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
#include "../channel/Channel.hpp"
#include "../reply/Reply.hpp"

class Server {
 private:
	 // TODO(hngouchi): あとで実装する。
	 // Config						config_;
	 int							socketFd_;
	 // socklen_tはコンパイラによってエラー。intの場合もある。
	 socklen_t					socketAddressLen_;
	 const int					maxClients_;
	 struct sockaddr_in			socketAddress_;
	 // TODO(hnoguchi): 定数で配列のサイズを指定する。
	 std::vector<User>			users_;
	 struct pollfd				fds_[7];
	 // TODO(hnoguchi): std::mapの方が良い？
	 std::vector<Channel>		channels_;

	 void	initializeServerSocket(unsigned short port);
	 void	initializeClientSockets();

	 void	handleServerSocket();
	 void	handleStandardInput();
	 void	handleClientSocket();
	 void	handleReceivedData(int clientIndex);

 public:
	 explicit Server(unsigned short port);
	 ~Server();
	 void	run(void);
};

ssize_t	sendNonBlocking(int fd, const char* buffer, size_t dataSize);

#endif  // SERVER_HPP
