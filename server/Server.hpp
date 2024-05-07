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
	 const ServerSocket		socket_;
	 Info					info_;
	 struct pollfd			fds_[MAX_FD];

	 int	setFd(int fd);
	 void	handleServerSocket();
	 // void	handleStandardInput();
	 void	handleClientSocket();
	 void	handleReceivedData(User* user);

 public:
	 explicit Server(unsigned short port);
	 ~Server();
	 // void	setPassword(const std::string& password);
	 void	run(void);

	 // debug
	 void	printData() const;
	// class GradeTooHighException : public std::out_of_range {
	// public:
	// 	GradeTooHighException(const std::string& msg = "Grade is too high.");
	// };
	// .cpp
	// Bureaucrat::GradeTooHighException::GradeTooHighException(const std::string& msg) : std::out_of_range(msg) {}
};

void	sendNonBlocking(int fd, const char* buffer, size_t dataSize);

#endif  // SERVER_HPP
