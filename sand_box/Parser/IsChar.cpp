#include <iostream>
// #include <iomanip>
// #include <sstream>
#include "./IsChar.hpp"
#include "../../color.hpp"

// bool	IsChar::isNotAlpha(const char c) {
// 	std::locale	l = std::locale::classic();
// 	return (!std::isalpha(c, l));
// }
// bool	IsChar::isNotDigit(const char c) {
// 	std::locale	l = std::locale::classic();
// 	return (!std::isdigit(c, l));
// }

// special	=	%x5B-60 / %x7B-7D	; "[", "]", "\", "`", "_", "^", "{", "|", "}"
bool	IsChar::isSpecial(const char c) {
	if (c >= 91 && c <= 96) {
		return (true);
	}
	if (c >= 123 && c <= 125) {
		return (true);
	}
	return (false);
}

// any octet except NUL, CR, LF, " "
bool	IsChar::isNullCrLfSpace(const char c) {
	if (c == 0 || c == 10 || c == 13 || c == 32) {
		return (true);
	}
	return (false);
}

// nospcrlfcl = %x01-09 / %x0B-0C / %x0E-1F / %x21-39 / %x3B-FF
// ; any octet except NUL, CR, LF, " " and ":"
bool	IsChar::isNoSpCrLfCl(const char c) {
	if (this->isNullCrLfSpace(c) || c == ':') {
		return (true);
	}
	return (false);
}

// chanstring = %x01-07 / %x08-09 / %x0B-0C / %x0E-1F / %x21-2B / %x2D-39 / %x3B-FF
// ; any octet except NUL, BELL, CR, LF, " ", "," and ":"
bool	IsChar::isChanStringChar(const char c) {
	if (this->isNoSpCrLfCl(c) || c == 7 || c == ',') {
		return (false);
	}
	return (true);
}

bool	IsChar::isFuncString(const std::string &str, bool (*func)(const char)) {
	try {
		return (std::find_if(str.begin(), str.end(), func) == str.end());
	} catch (std::exception& e) {
		throw;
	}
}

#ifdef UTEST_ISCHAR
void	test(IsChar* obj, bool (IsChar::*func)(const char)) {
	std::cout.unsetf(std::ios::showbase);
	std::cout.setf(std::ios::uppercase);
	std::cout.setf(std::ios::hex, std::ios::basefield);
	for (char c = 0; c < 16; c++) {
		if ((obj->*func)(c)) {
			std::cout << GREEN << "0X0" << (int)c << END << ", " << std::flush;
		} else {
			std::cout << RED << "0X0" << (int)c << END << ", " << std::flush;
		}
	}
	std::cout.setf(std::ios::showbase);
	for (char c = 16; c < 127; c++) {
		if (c % 16 == 0) {
			std::cout << std::endl;
		}
		if ((obj->*func)(c)) {
			std::cout << GREEN << (int)c << END << ", " << std::flush;
		} else {
			std::cout << RED << (int)c << END << ", " << std::flush;
		}
	}
	if ((obj->*func)(127)) {
		std::cout << GREEN << 127 << END << std::flush;
	} else {
		std::cout << RED << 127 << END << std::flush;
	}
	std::cout << std::endl;
}

int main() {
	IsChar	obj;
	bool (IsChar::*fptr)(const char);
	// std::cout << "[isNotAlpha] ____________________" << std::endl;
	// test(validChar.isNotAlpha);
	// std::cout << "_________________________________" << std::endl;

	// std::cout << "[isNotDigit] ____________________" << std::endl;
	// test(validChar.isNotDigit);
	// std::cout << "_________________________________" << std::endl;

	std::cout << "[isSpecial] ________________________" << std::endl;
	std::cout << " 0X5B-0X60 / 0X7B-0X7D ; [, ], \\, `, _, ^, {, |, }" << std::endl;
	// bool (IsChar::*fptr)(const char) = &IsChar::isSpecial;
	fptr = &IsChar::isSpecial;
	test(&obj, fptr);
	std::cout << "_________________________________" << std::endl;

	std::cout << "[isNullCrLfSpace] _______________" << std::endl;
	std::cout << "any octet except NUL, CR, LF, \" \"" << std::endl;
	fptr = &IsChar::isNullCrLfSpace;
	test(&obj, fptr);
	std::cout << "_________________________________" << std::endl;
	
	std::cout << "[isNoSpCrLfCl] __________________" << std::endl;
	std::cout << "0X01-0X09 / 0X0B-0X0C / 0X0E-0X1F / 0X21-0X39 / 0X3B-0XFF" << std::endl;
	std::cout << "any octet except NUL, CR, LF, \" \" and : " << std::endl;
	fptr = &IsChar::isNoSpCrLfCl;
	test(&obj, fptr);
	std::cout << "_________________________________" << std::endl;
	std::cout << "[isChanStringChar] ______________" << std::endl;
	std::cout << " 0X01-0X07 / 0X08-0X09 / 0X0B-0X0C / 0X0E-0X1F / 0X21-0X2B / 0X2D-0X39 / 0X3B-0XFF " << std::endl;
	std::cout << "any octet except NUL, BELL, CR, LF, \" \", \",\" and :" << std::endl;
	fptr = &IsChar::isChanStringChar;
	test(&obj, fptr);
	std::cout << "_________________________________" << std::endl;
	std::cout << "any octet except NUL, CR, LF, \" \" and @" << std::endl;
#ifdef DEBUG
	system("leaks validchar");
#endif  // DEBUG
}

#endif  // UTEST_ISCHAR
