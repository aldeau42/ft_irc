#include "Server.hpp"

int main(int argc, char **argv)
{
	Server serv;
	if (argc != 3) {
		std::cerr << RED << "Usage: <./ircserv> <Port> <Password>" << WHI << std::endl;
		return (1);
	}
	try {
		signal(SIGINT, Server::SignalHandler);
		signal(SIGQUIT, Server::SignalHandler);
		serv.ServerInit(argv);
	}
	catch(const std::exception& e) {
		serv.CloseFds();
		std::cerr << RED << e.what() << WHI << std::endl;
	}
	std::cout << "The Server Closed!" << std::endl;
	return 0;
}
