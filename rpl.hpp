#pragma once

#include <iostream>
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

std::string ERR_NONICKNAMEGIVEN(std::string nickname);
std::string ERR_NICKNAMEINUSE(std::string nickname);
std::string ERR_NONICKCHANGE(std::string nickname);
std::string ERR_NOTREGISTERED(std::string nickname);
std::string ERR_NEEDMODEPARM(std::string command);
std::string ERR_ALREADYREGISTERED(std::string nickname);
std::string ERR_PASSWDMISMATCH(std::string nickname);
std::string ERR_USERONCHANNEL(std::string nickname, std::string channel);

std::string ERR_NOSUCHCHANNEL(std::string chan);
std::string ERR_INVITEONLYCHAN(std::string nickname, std::string chan);
std::string ERR_NOSUCHNICK(std::string nickname);
std::string ERR_NOTONCHANNEL(std::string nickname);
std::string ERR_CHANOPRIVSNEEDED(std::string nickname, std::string chan);

std::string KICKING(std::string nickname, std::string username, std::string channel, std::string kickedname);
std::string KICKING_COM(std::string nickname, std::string username, std::string channel, std::string kickedname, std::string comment);

std::string serv329(std::string serverName, std::string NameChanel, std::string nickname);
std::string serv331(std::string serverName, std::string NameChanel, std::string nickname);
std::string serv332(std::string serverName, std::string NameChanel, std::string nickname, std::string topic);
std::string serv333(std::string serverName, std::string NameChanel, std::string nickname, std::string topicSetter);
std::string Serv341(std::string serverName, std::string nickname, std::string username, std::string channel);
std::string Serv342(std::string serverName, std::string nickname, std::string username, std::string channel);
std::string serv353(std::string serverName, std::string sender_nickname, std::string NameChanel, std::string nickname, int isOp);
std::string serv366(std::string serverName, std::string NameChanel, std::string nickname);
std::string serv441(std::string serverName, std::string NameChanel, std::string nickname, std::string user);
std::string serv442(std::string serverName, std::string NameChanel, std::string nickname);
std::string serv471(std::string serverName, std::string NameChanel, std::string nickname);
std::string serv482(std::string serverName, std::string NameChanel, std::string nickname);

