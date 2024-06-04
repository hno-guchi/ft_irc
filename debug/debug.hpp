#ifndef DEBUG_HPP
# define DEBUG_HPP

#include <iostream>
#include <string>

void	debugPrintSendMessage(const std::string& to, const std::string &message);
void	debugPrintRecvMessage(const std::string& from, const std::string &message);
void	debugPrintErrorMessage(const std::string &message);

#endif  // DEBUG_HPP

