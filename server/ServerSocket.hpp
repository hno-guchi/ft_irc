#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>

class ServerSocket {
 private:
	 int					fd_;
	 struct sockaddr_in		address_;
	 // socklen_tはコンパイラによってエラー。intの場合もある。
	 socklen_t				addressLen_;
 public:
	 explicit ServerSocket(unsigned short port);
	 ~ServerSocket();
	 // GETTERS
	 int	getFd() const;
	 int	createClientSocket() const;
};

#endif  // SERVERSOCKET_HPP
