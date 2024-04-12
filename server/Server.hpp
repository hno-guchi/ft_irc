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

#define MAX_FD 7

class Server {
 private:
	 const ServerSocket		socket_;
	 Info					info_;
	 struct pollfd			fds_[MAX_FD];

	 void	handleServerSocket();
	 void	handleStandardInput();
	 void	handleClientSocket();
	 void	handleReceivedData(int i);

 public:
	 explicit Server(unsigned short port);
	 ~Server();
	 void	run(void);

	// class GradeTooHighException : public std::out_of_range {
	// public:
	// 	GradeTooHighException(const std::string& msg = "Grade is too high.");
	// };
	// .cpp
	// Bureaucrat::GradeTooHighException::GradeTooHighException(const std::string& msg) : std::out_of_range(msg) {}
};

ssize_t	sendNonBlocking(int fd, const char* buffer, size_t dataSize);

#endif  // SERVER_HPP
