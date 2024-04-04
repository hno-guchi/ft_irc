#ifndef SERVER_HPP
# define SERVER_HPP

#include <poll.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cstring>
#include "./ServerSocket.hpp"
#include "./Config.hpp"
#include "./Info.hpp"
// #include "../user/User.hpp"
// #include "../channel/Channel.hpp"
#include "../reply/Reply.hpp"

class Server {
 private:
	 const ServerSocket		socket_;
	 Info					info_;
	 // const Config			config_;
	 // TODO(hnoguchi): 定数で配列のサイズを指定する。
	 // std::vector<User>		users_;
	 // TODO(hnoguchi): std::mapの方が良い？
	 // std::vector<Channel>	channels_;
	 struct pollfd			fds_[7];

	 void	handleServerSocket();
	 void	handleStandardInput();
	 void	handleClientSocket();
	 void	handleReceivedData(int i);

 public:
	 explicit Server(unsigned short port);
	 ~Server();
	 void	run(void);
};

ssize_t	sendNonBlocking(int fd, const char* buffer, size_t dataSize);

#endif  // SERVER_HPP
