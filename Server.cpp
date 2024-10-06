#include "Server.hpp"

Server::Server()
{
	SerSocketFd = -1;
	serverName = "serveur_irc";
	version = "1.0.0";
	creationDate = "12-06-2024";
	userModes = "i";
	channelModes = "m";
	_passWord = "";
}


//////////////////////////////////////////////////////////////////////////////////////////////////
/// CLIENTS & SOCKETS GESTION ///
//////////////////////////////////////////////////////////////////////////////////////////////////

int Server::GetClientInfo(int fd){
	for(size_t i = 0; i < clients.size(); i++){
		if (fd == clients[i].GetFd())
			return i;
	}
	return -1;
}

Client& Server::getClientByFd(int fd) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i].GetFd() == fd) {
            return clients[i];
        }
    }
    throw std::runtime_error("Client with the specified file descriptor not found.");
}

Client& Server::getClientByNickname(std::string nickname) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i].GetNickname() == nickname) {
            return clients[i];
        }
    }
    throw std::runtime_error("Client with the specified file descriptor not found.");
}

void Server::ClearClients(int fd){ 
	for(size_t i = 0; i < fds.size(); i++){
		if (fds[i].fd == fd)
			{fds.erase(fds.begin() + i); break;}
	}
	for(size_t i = 0; i < clients.size(); i++){
		if (clients[i].GetFd() == fd)
			{clients.erase(clients.begin() + i); break;}
	}
	for(size_t i = 0; chanelles.size() > i; i++)
	{
		chanelles[i].removeClientChannelByFd(fd);
	}
}

void	Server::CloseFds() {
	for(size_t i = 0; i < clients.size(); i++){
		std::cout << RED << "Client <" << clients[i].GetNickname() << "> Disconnected" << WHI << std::endl;
		close(clients[i].GetFd());
	}
	if (SerSocketFd != -1){
		std::cout << RED << "Server <" << SerSocketFd << "> Disconnected" << WHI << std::endl;
		close(SerSocketFd);
	}
}

void Server::connecting(Client& client){
	std::cout << YEL << "name :" << client.GetUsername() << "\tnick :" << client.GetNickname() << "\tpass :" << client.GetPass() << WHI << std::endl;
	if (!client.GetNickname().empty() && !client.GetUsername().empty() && client.GetPass() == _passWord){
		client.SetReg(true);
		sendWelcomeMessages(client);
	}
	else
		client.SetReg(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/// SIGNAL ///
//////////////////////////////////////////////////////////////////////////////////////////////////

bool Server::Signal = false;
void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::Signal = true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
/// DATA --- SEND & RECEIVE ///
//////////////////////////////////////////////////////////////////////////////////////////////////

void Server::SendNewData(int fd, char* buff)
{
	std::string message = "Client <" + clients[GetClientInfo(fd)].GetNickname() + "> " + buff;
	for(size_t i = 0; i < clients.size(); i++){
		if (fd != clients[i].GetFd())
			ToSend(clients[i].GetFd(), message.c_str());
	}
}

void Server::ReceiveNewData(int fd)
{
	(void) fd;
	char buff[1024]; 
	memset(buff, 0, sizeof(buff));

	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1 , 0);
	std::cout << "*-*" << buff << "***" << std::endl;
	if (bytes <= 0){
		if (clients[GetClientInfo(fd)].GetNickname().empty())
			std::cout << RED << "Client <" << clients[GetClientInfo(fd)].GetFd() << "> Disconnected" << WHI << std::endl;
		else
			std::cout << RED << "Client <" << clients[GetClientInfo(fd)].GetNickname() << "> Disconnected" << WHI << std::endl;
		ClearClients(fd);
		close(fd);
	}
	else {
		Client& client = getClientByFd(fd);
		client.addCommand(buff);
		if (client.fullCommand()){
			buff[bytes] = '\0';
			ClientCommand(fd, client.GetCommand());
			client.delCommand();
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////
/// COMMANDS ///
//////////////////////////////////////////////////////////////////////////////////////////////////

bool Server::ClientCommand(int fd, std::string command) 
{
	t_f f[] = { &Server::Ping, &Server::Nick, &Server::User, &Server::Join, &Server::Msg, &Server::Kick, &Server::Invite, &Server::Topic, &Server::Mode, &Server::Pass, &Server::Quit};
	command = remove_unprintable(command);
    std::vector<std::string> parts = split(command, ' ');

    if (parts.size() <= 1) {
        std::cout << "IRC: Not enough parameters" << std::endl;
        return false;
    }
    std::string extractedCommand = parts[0];
	for (unsigned long i = 0; i < parts.size(); i++) {
		std::cout << "here extracted_cmd*-*" << parts[i] << "\n*-*\n" << std::endl;
	}
    std::string ask[11] = {"PING", "NICK", "USER", "JOIN", "PRIVMSG", "KICK", "INVITE", "TOPIC", "MODE", "PASS", "QUIT"};
    for (int i = 0; i < 11; i++) {
        if (extractedCommand == ask[i]) {
            (this->*f[i])(fd, parts);
            return true;
		}
	}
    return false;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
/// SERVER INIT & POLLFD ///
//////////////////////////////////////////////////////////////////////////////////////////////////

void Server::GetInfoCli(Client &cli, std::string buff) {
	int passStart = buff.find("PASS") + 5;
	std::string pass = buff.substr(passStart, buff.find(" ", passStart) - passStart);
	std::string nick = buff.substr(buff.find("NICK") + 5,  buff.find("\n") - 2);
	std::string user = buff.substr(buff.find(":",buff.find("USER") + 5) + 1);

	std::vector<std::string> command;
	cli = getClientByFd(cli.GetFd());
	command.push_back("COMM");
	command.push_back(remove_unprintable(user));
	User(clients.back().GetFd(), command);
	command.pop_back();
	command.push_back(remove_unprintable(nick));
	Nick(clients.back().GetFd(), command);
	command.pop_back();
	command.push_back(remove_unprintable(pass));
	Pass(clients.back().GetFd(), command);
	if (clients.back().GetPass().empty())
	{
		std::cout << RED << "Client <" << clients.back().GetNickname() << "> Disconnected" << WHI << std::endl;
		ClearClients(cli.GetFd());
		close(cli.GetFd());
		cli.setRefuse(true);
		return;
	}
}

void Server::initClient(Client &cli, int incofd) {
    cli.SetFd(incofd); 
    cli.SetUsername(""); 

    char buffer[1024];
    std::string data;
    ssize_t bytesRead;
	int flag_irsii = 0;
	int i = 0;
	bool hasCap = false, hasPass = false, hasNick = false, hasUser = false;

    while (!(hasCap && hasPass && hasNick && hasUser)) {
		bytesRead = recv(incofd, buffer, sizeof(buffer) - 1, 0);
		
		if (bytesRead > 0) {
			buffer[bytesRead] = '\0';
			flag_irsii++;;
            data += buffer;
			if (data.find("CAP LS") != std::string::npos) hasCap = true;
            if (data.find("PASS") != std::string::npos) hasPass = true;
            if (data.find("NICK") != std::string::npos) hasNick = true;
            if (data.find("USER") != std::string::npos) hasUser = true;
        }
		if (i == 100) {
			if (flag_irsii != 0)
				break;
			return;
		}
		i++;
	}
	GetInfoCli(cli, data);
}

void Server::sendWelcomeMessages(Client& cli) 
{
    std::string WELCOME = ":server 001 " + cli.GetNickname() + " :Welcome to " + serverName + " Network, " + cli.GetNickname() + "[!" + cli.GetUsername() + "@" + cli.getIpAdd() + "]\r\n";
    std::string YOURHOST = ":server 002 " + cli.GetNickname() + " :Your host is " + serverName + ", running version " + version + "\r\n";
    std::string CREATED = ":server 003 " + cli.GetNickname() + " :This server was created " + creationDate + "\r\n";
    std::string MYINFO = ":server 004 " + cli.GetNickname() + " " + serverName + " " + version + " " + userModes + " " + channelModes + "\r\n";

    ToSend(cli.GetFd(), WELCOME.c_str());
    ToSend(cli.GetFd(), YOURHOST.c_str());
    ToSend(cli.GetFd(), CREATED.c_str());
    ToSend(cli.GetFd(), MYINFO.c_str());
}

void Server::AcceptNewClient()
{
	Client cli;
	struct sockaddr_in cliadd;
	struct pollfd NewPoll;
	socklen_t len = sizeof(cliadd);

	int incofd = accept(SerSocketFd, (sockaddr *)&(cliadd), &len);
	if (incofd == -1)
	{
		std::cout << "accept() failed" << std::endl;
		return;
	}
	if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "fcntl() failed" << std::endl;
		return;
	}
	NewPoll.fd = incofd;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;

	cli.SetFd(incofd);
	cli.setIpAdd(inet_ntoa((cliadd.sin_addr)));
	clients.push_back(cli);
	std::cout << GRE << "Client <" << incofd << "> Connected" << WHI << std::endl;
    initClient(cli, incofd);
	if (cli.getRefuse() == false)
		fds.push_back(NewPoll);
}

void Server::SerSocket()
{
	int en = 1;
	struct sockaddr_in add;
	struct pollfd NewPoll;
	add.sin_family = AF_INET;
	add.sin_addr.s_addr = INADDR_ANY;
	add.sin_port = htons(this->Port);

	SerSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if(SerSocketFd == -1)
		throw(std::runtime_error("failed to create socket"));

	if(setsockopt(SerSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("failed to set option (SO_REUSEADDR) on socket"));
	 if (fcntl(SerSocketFd, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("failed to set option (O_NONBLOCK) on socket"));
	if (bind(SerSocketFd, (struct sockaddr *)&add, sizeof(add)) == -1)
		throw(std::runtime_error("failed to bind socket"));
	if (listen(SerSocketFd, SOMAXCONN) == -1)
		throw(std::runtime_error("listen() failed"));

	NewPoll.fd = SerSocketFd;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;
	fds.push_back(NewPoll);
}

void Server::ServerInit(char **argv)
{
	this->Port = atoi(argv[1]);
	if (this->Port < 1024 || 49152 < this->Port)
		throw(std::logic_error("Wrong Port number [1024 <= Port <= 49152]"));
	this->_passWord = argv[2];
	SerSocket();

	std::cout << GRE << "Server <" << SerSocketFd << "> Connected" << WHI << std::endl;
	std::cout << "Waiting to accept a connection...\n";

	while (Server::Signal == false)
	{

		if((poll(&fds[0],fds.size(),-1) == -1) && Server::Signal == false)
			throw(std::runtime_error("poll() faild"));

		for (size_t i = 0; i < fds.size(); i++){
			if (fds[i].revents & POLLIN){
				if (fds[i].fd == SerSocketFd)
					AcceptNewClient();
				else
					ReceiveNewData(fds[i].fd);
			}
		}
	}
	CloseFds();
}
