#pragma once

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h> 
#include <netinet/in.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <poll.h> 
#include <csignal> 
#include <sstream>
#include <cerrno>
#include <cstdlib>
#include <algorithm>
#include <map> 
#include "Client.hpp"
#include "Channel.hpp"
#include "rpl.hpp"

//-------------------------------------------------------//

#define RED "\e[1;31m" 
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"
#define BLU "\e[1;34m"
#define VIO "\e[1;35m"
#define CYA "\e[1;36m"
#define WHI "\e[0;37m"

//-------------------------------------------------------//

class Command;

class Server 
{
	private:
		int 		Port;
		int 		SerSocketFd;
		static bool Signal;
		std::vector<Channel> 		chanelles;
		std::vector<Client> 		clients;
		std::vector<struct pollfd> 	fds;
		std::string 				_passWord;

	public:
		Server(); 

	    std::string serverName;
    	std::string version;
  	  	std::string creationDate;
   		std::string userModes;
	    std::string channelModes;

		void ServerInit(char **argv);
		void SerSocket();
		void AcceptNewClient();
		void ReceiveNewData(int fd);

		void SendNewData(int fd, char* buff);
		static void SignalHandler(int signum);
		void initClient(Client &cli, int incofd);
		int GetClientInfo(int fd);
		bool ClientCommand(int fd, std::string command);
		Client& getClientByFd(int fd);
		Client& getClientByNickname(std::string nickname);
		void GetInfoCli(Client &cli, std::string buff);
		void CloseFds();
		void ClearClients(int fd);
		bool already_used_nick(std::string nick);
		void addChannel(std::string nameChanel);
		void sendWelcomeMessages(Client& cli);
		void delChannel(); 


        void Ping(int fd, const std::vector<std::string>& parts);
        void Join(int fd, const std::vector<std::string>& parts);
		void Join2(int fd, std::string params);
		void sendJoinMessages(int fd, std::string channel, Client& sender, int isChannel);
        void Msg(int fd, const std::vector<std::string>& parts);
        void Kick(int fd, const std::vector<std::string>& parts);
        void User(int fd, const std::vector<std::string>& parts);
        void Nick(int fd, const std::vector<std::string>& parts);
        void Invite(int fd, const std::vector<std::string>& parts);
        void Topic(int fd, const std::vector<std::string>& parts);
		void Quit(int fd, const std::vector<std::string>& parts);
        void Pass(int fd, const std::vector<std::string>& parts);
        void Mode(int fd, const std::vector<std::string>& parts);
		void ModeI(Client& sender, int isChannel, bool isOperator, bool add);
		void ModeT(Client& sender, int isChannel, bool isOperator, bool add);
		void ModeK(Client& sender, int isChannel, bool isOperator, bool add, const std::vector<std::string>& parts);
		bool ModeO(Client& sender, int isChannel, bool isOperator, bool add, const std::vector<std::string>& parts);
		void ModeL(Client& sender, int isChannel, bool isOperator, bool add, const std::vector<std::string>& parts);

		void ToSend(int fd, std::string msg);
		void ToSendServer(std::string msg);

		bool checkReg(int fd);
		bool checkParams(int fd, const std::vector<std::string>& params, size_t nb);
		bool checkTopic( Channel& channel);
		bool checkKey(int isChannel, const std::vector<std::string>& parts);
		int	checkChannel(std::string channel);
		int checkNick(std::string nickname);
		std::vector<std::string> checkCommasCmd(std::string str);

		std::vector<std::string> split(const std::string& s, char delimiter);
		bool checkNumber(const std::string& s);
		std::string remove_unprintable(std::string str);
		std::string onlyParams(const std::vector<std::string>& parts);
		void connecting(Client& client);
};

std::string extractNickname(const std::string& data);
typedef void (Server::*t_f)(int, const std::vector<std::string>&);
