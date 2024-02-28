#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>

class Client {
 private:
	 int					socketFd_;
	 struct sockaddr_in		serverSocketAddress_;

 public:
	 Client(const std::string& serverIP, unsigned short serverPort);
	 ~Client();

	 void		connectToServer();
	 ssize_t	sendMessage(const std::string& message);
	 ssize_t	receiveMessage();
};

#endif  // CLIENT_HPP
