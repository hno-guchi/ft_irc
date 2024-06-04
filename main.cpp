#include <signal.h>
#include "./color.hpp"
#include "./server/Server.hpp"

static void	fatalError(const std::string& message) {
	std::cerr << errno << " : " << std::flush;
	std::perror(message.c_str());
	exit(EXIT_FAILURE);
}

void	signalHandler(int signal, void (*handler)(int)) {
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
void	setSignals() {
	signalHandler(SIGINT, SIG_DFL);
	signalHandler(SIGTERM, SIG_DFL);
	signalHandler(SIGQUIT, SIG_DFL);

	signalHandler(SIGHUP, SIG_IGN);
	signalHandler(SIGINT, SIG_IGN);
	signalHandler(SIGPIPE, SIG_IGN);
	signalHandler(SIGUSR1, SIG_IGN);
	signalHandler(SIGUSR2, SIG_IGN);
}

// TODO(hnoguchi): サーバーの終了処理を実装する。
// TODO(hnoguchi): <port>, <password>のバリデーションを実装する。
int	main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		// return(1);
	}
	setSignals();
	try {
		(void)argv;
		Server	Server(8080);

		Server.run();
	} catch (std::exception& e) {
		fatalError(e.what());
	}
	return (0);
}

// #ifdef SERVER_LEAKS
// __attribute__((destructor)) static void destructor() {
//     system("leaks -q ircserv");
// }
// #endif  // SERVER_LEAKS
