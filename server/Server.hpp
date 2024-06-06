#ifndef SERVER_HPP
# define SERVER_HPP

#include <poll.h>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstring>
#include "./ServerSocket.hpp"
#include "./Config.hpp"
#include "./Info.hpp"
#include "../reply/Reply.hpp"
#include "../user/User.hpp"

// #define MAX_FD 7
#define MAX_FD 6

class Server {
 private:
	 const ServerSocket			socket_;
	 Info						info_;
	 struct pollfd				fds_[MAX_FD];

	 int						setFd(int fd);
	 void						handleServerSocket();
	 void						handleClientSocket();
	 void						handleReceivedData(User* user);
	 std::vector<std::string>	split(const std::string& message, User* user);

	 explicit Server(const Server& rhs);
	 Server& operator=(const Server& rhs);

 public:
	 explicit Server(unsigned short port = 8080, const std::string& connectPwd = "");
	 ~Server();

	 void			run(void);
	 static void	sendNonBlocking(int fd, const char* buffer, size_t dataSize);

	 // debug
	 void	debugPrintServer() const;
};

#endif  // SERVER_HPP
