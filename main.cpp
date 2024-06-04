#include <signal.h>
#include "./color.hpp"
#include "./server/Server.hpp"

static void	fatalError(const std::string& message) {
	std::cerr << errno << " : " << std::flush;
	std::perror(message.c_str());
	exit(EXIT_FAILURE);
}

// TODO(hnoguchi): サーバーの終了処理を実装する。
// TODO(hnoguchi): Server classにpasswordを追加する。
// TODO(hnoguchi): <port>, <password>のバリデーションを実装する。
int	main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		// return(1);
	}

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
