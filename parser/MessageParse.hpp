#ifndef MESSAGEPARSE_HPP
# define MESSAGEPARSE_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class MessageParse {
	private:
		std::vector<std::string>	tokens_;
		std::string					message_;
	public:
		explicit MessageParse(const std::string &message);
		~MessageParse();

		void	tokenize();
		// debug
		void	printTokens() const;
};

#endif  // MESSAGEPARSE_HPP
