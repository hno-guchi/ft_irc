#include "./Parser.hpp"
#include "../../color.hpp"

#ifdef UTEST_TOKENIZE

int main() {
	std::string	testList[] = {
		"COMMAND",
		"COMMAND param1",
		"COMMAND param1 param2",
		"COMMAND param1 param2 param3",
		"COMMAND param1 param2 param3 param4",
		"COMMAND param1 param2 param3 param4 param5",
		"",
		" ",
		"  ",
		"   ",
		"    ",
		"     ",
		" COMMAND",
		"  COMMAND",
		"   COMMAND",
		"    COMMAND",
		"     COMMAND",
		"COMMAND ",
		"COMMAND  ",
		"COMMAND   ",
		"COMMAND    ",
		"COMMAND     ",
		" COMMAND param1",
		"    COMMAND param1",
		"COMMAND  param1",
		"COMMAND   param1",
		"COMMAND    param1",
		"COMMAND     param1",
		"COMMAND param1 ",
		"COMMAND param1  ",
		"COMMAND param1   ",
		"COMMAND param1    ",
		"COMMAND param1     ",
		"COMMAND param1  param2",
		"COMMAND param1   param2",
		"COMMAND param1    param2",
		"COMMAND param1     param2",
		"COMMAND :param1",
		"COMMAND :param1 param2",
		"COMMAND param1 :param2",
		"COMMAND param1 param2 :param3 param4 param5",
		"COMMAND  :param1",
		"COMMAND   :param1",
		"COMMAND    :param1",
		"COMMAND     :param1",
		"COMMAND : param1",
		"COMMAND :  param1",
		"COMMAND :   param1",
		"COMMAND :    param1",
		"COMMAND :     param1",
		"COMMAND :",
		"COMMAND : ",
		"COMMAND :  ",
		"COMMAND :   ",
		"COMMAND :    ",
		"COMMAND :     ",

	};
	for (size_t i = 0; i < sizeof(testList) / sizeof(testList[0]); i++) {
		// std::cout << "[Message]    ____________________" << std::endl;
		std::cout << "[" << GREEN << testList[i] << END << "]" << std::endl;
		// std::cout << "_________________________________" << std::endl;

		Parser				parser;
		std::vector<Token>	tokens;

		parser.tokenize(&testList[i], &tokens);
		parser.printTokens(tokens);
		// parser.getParsedMessage().printParsedMessage();
		// std::cout << YELLOW << "[Execute]   ____________________" << END << std::endl;
		// std::cout << YELLOW << "[Reply]     ____________________" << END << std::endl;
		// std::cout << std::endl;
	}

}

#endif  // UTEST_TOKENIZE

#ifdef UTEST_PARSE
const std::string	cmdList_[] = {
	"PASS",
	"NICK",
	"USER",
	"OPER",
	"QUIT",
	"JOIN",
	"PART",
	"KICK",
	"INVITE",
	"TOPIC",
	"MODE",
	"PRIVMSG",
	"NOTICE",
	"PING",
	""
};


int	main() {
	std::string	testList[] = {
		"PING irc.example.net",
		"PING :irc.example.net",
		"PING :irc.example.net :irc.example.net",
		"PING",
		"PASS password",
		"PASS password password2",
		"PASS :password password2",
		"PASS",
		"NICK nickname",
		"NICK nickname nickname2",
		"NICK :nickname nickname2",
		"NICK",
		"USER username hostname servername realname",
		"USER username hostname servername realname realname2",
		"USER :username hostname servername realname realname2",
		"USER",
		"OPER user password",
		"OPER user password password2",
		"OPER :user password password2",
		"OPER",
		"QUIT",
		"QUIT :see you later",
		"QUIT see you later :see you later",
		"JOIN #channel",
		"JOIN #channel key",
		"JOIN #channel key key2",
		"JOIN #channel :key key2",
		"JOIN",
		"JOIN 0",
		"PART #channel",
		"PART #channel :see you later",
		"PART #channel see you later :see you later",
		"PART",
		"KICK #channel nickname",
		"KICK #channel nickname :reason",
		"KICK #channel nickname reason :reason",
		"KICK",
		"INVITE nickname #channel",
		"INVITE nickname #channel :reason",
		"INVITE #channel reason",
		"INVITE",
		"TOPIC #channel",
		"TOPIC #channel :new topic",
		"TOPIC :new topic",
		"TOPIC #channel new :topic",
		"TOPIC #channel new topic",
		"TOPIC",
		"MODE nickname",
		"MODE nickname +r",
		"MODE nickname -r",
		"MODE nickname +o",
		"MODE nickname +oaaa",
		"MODE",
		"MODE #channel +i",
		"MODE #channel +k key",
		"MODE #channel +l 4",
		"MODE #channel +o nickname",
		"MODE #channel +t",
		"MODE #channel -i",
		"MODE #channel -k",
		"MODE #channel -l",
		"MODE #channel -o nickname",
		"MODE #channel -t",
		"MODE #channel tt",
		"MODE #channel tttt ddd sss",
		"MODE",
		"PRIVMSG nickname :message",
		"PRIVMSG #channel :message",
		"PRIVMSG :message",
		"PRIVMSG",
		"NOTICE nickname :message",
		"NOTICE #channel :message",
		"NOTICE :message",
		"NOTICE",
	};

	for (size_t i = 0; i < sizeof(testList) / sizeof(testList[0]); i++) {
		std::cout << "_________________________" << std::endl;
		std::cout << GREEN << testList[i] << END << std::endl;

		Parser	parser;

		parser.parse(testList[i], cmdList_);
		parser.getParsedMsg().printParsedMsg();
		std::cout << std::endl;
	}
#ifdef LEAKS
	system("leaks -q parser");
#endif  // LEAKS
	return(0);
}
#endif  // UTEST_PARSE
