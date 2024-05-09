#ifndef ISCHAR_HPP
# define ISCHAR_HPP

# include <string>

class IsChar {
 public:
	 // static bool	isNotAlpha(const char c);
	 // static bool	isNotDigit(const char c);
	 bool	isSpecial(const char c);
	 bool	isNullCrLfSpace(const char c);
	 bool	isNoSpCrLfCl(const char c);
	 bool	isChanStringChar(const char c);
	 bool	isFuncString(const std::string &str, bool (*func)(const char));
};

#endif  // ISCHAR_HPP
