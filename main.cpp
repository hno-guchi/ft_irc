#include <signal.h>
#include "./color.hpp"
#include "./server/Server.hpp"

static void	fatalError(const std::string& message) {
	std::perror(message.c_str());
	exit(EXIT_FAILURE);
}

int	convertStringToInt(const std::string& str) {
	try {
		std::stringstream	ss(str);
		int					ret;

		ss >> ret;
		if (ss.fail()) {
			throw std::runtime_error("Failed to convertStringToInt();");
		}
		return (ret);
	} catch (const std::exception& e) {
		throw;
	}
}

static void	signalHandler(int signal, void (*handler)(int)) {
	struct sigaction	sa;

	std::memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	if (sigemptyset(&sa.sa_mask) == -1) {
		fatalError("sigemptyset");
	}
	if (sigaction(signal, &sa, NULL) == -1) {
		fatalError("sigaction");
	}
}

// TODO(hnoguchi): 適切な実装か確認すること。
static void	setSignals() {
	signalHandler(SIGINT, SIG_DFL);
	signalHandler(SIGTERM, SIG_DFL);
	signalHandler(SIGQUIT, SIG_DFL);

	signalHandler(SIGHUP, SIG_IGN);
	signalHandler(SIGINT, SIG_IGN);
	signalHandler(SIGPIPE, SIG_IGN);
	signalHandler(SIGUSR1, SIG_IGN);
	signalHandler(SIGUSR2, SIG_IGN);
}

static bool	isNotDigit(char c) {
	return (!std::isdigit(c));
}

static bool	isPort(const std::string& port) {
	if (port.empty()) {
		return (false);
	}
	if (std::find_if(port.begin(), port.end(), isNotDigit) != port.end()) {
		return (false);
	}
	return (true);
}

static bool isPassword(const std::string& pwd) {
	if (pwd.empty()) {
		return (false);
	}
	if (pwd.size() > 10) {
		return (false);
	}
	return (true);
}

// TODO(hnoguchi): サーバーの終了処理を実装する。
int	main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return(1);
	}
	setSignals();
	try {
		std::string	port(argv[1]);
		std::string	connectPwd(argv[2]);

		if (!isPort(port)) {
			fatalError("isPort");
		}
		if (!isPassword(connectPwd)) {
			fatalError("isPassword");
		}
		int	portNum = convertStringToInt(argv[1]);
		Server	Server(portNum, connectPwd);
		Server.run();
	} catch (std::exception& e) {
		fatalError(e.what());
	}
#ifdef LEAKS
	system("leaks -q ircserv");
#endif  // LEAKS
	return (0);
}

// #ifdef SERVER_LEAKS
// __attribute__((destructor)) static void destructor() {
//     system("leaks -q ircserv");
// }
// #endif  // SERVER_LEAKS
