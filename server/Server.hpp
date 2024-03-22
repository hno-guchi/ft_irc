#ifndef SERVER_HPP
# define SERVER_HPP

#include <poll.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cstring>
#include "./ServerSocket.hpp"
#include "../user/User.hpp"
#include "../channel/Channel.hpp"
#include "../reply/Reply.hpp"

class Server {
 private:
	 ServerSocket					socket_;
	 // TODO(hngouchi): あとで実装する。
	 // Config						config_;
	 const int					maxClients_;
	 // TODO(hnoguchi): 定数で配列のサイズを指定する。
	 std::vector<User>			users_;
	 struct pollfd				fds_[7];
	 // TODO(hnoguchi): std::mapの方が良い？
	 std::vector<Channel>		channels_;

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
