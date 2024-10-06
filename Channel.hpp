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
#include <sstream>
#include <cerrno>
#include <cstdlib>
#include <algorithm> 
#include "Client.hpp"
//-------------------------------------------------------//

#define RED "\e[1;31m" 
#define WHI "\e[0;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"
//-------------------------------------------------------//

class Client;

class Channel
{
    private:
        std::vector<Client> 	_channelClients;
        std::vector<Client> 	_operatorNicknames;
        std::vector<Client> 	_invitedList;
        std::string 		_channelName;
        std::string 		_topic;
        std::string 		_topicSetter;
        
        bool 		    _i;
        bool 		    _t;
        std::string 	_key;
        bool		    _needKey;
        std::string 	_operatorNick;
        size_t      	_limit;

    public:
        Channel(std::string name_arg);
        ~Channel();

        void setChannelName(std::string newName);
        void setTopic(std::string newTopic, std::string nickname);
        void setNeedKey(bool needKey);
        
        std::string getChannelName();      
        size_t      getChannelClientsCount();
        bool        getNeedKey();            
        std::string getTopic();       
        std::string getTopicSetter();
        std::string getModes();
        std::vector<Client> getChannelClients(); 

        void setI(bool i);
        void setT(bool t);              
        void setK(std::string key);
        void setL(size_t limit);          
        bool                getI();
        bool                getT();
        std::string         getK();
        size_t              getL();
        void eraseCli(size_t x);

        void addClientToChannel(Client& cli);
        void removeClientChannelByFd(int fd);
        bool checkClientChannel(std::string nickname);
        std::vector<std::string> clientsList();
        
        void addOperatorToChannel(Client& cli);
        void removeOperatorByFd(int fd);
        bool checkOperator(std::string nickname);
        bool isOperator();
        void operatorsList();

        void addInvitedToList(Client& cli);
        void removeInvitedFromList(std::string nickname);        
        bool isInvited(std::string nickname);
        void invitedList();
        
        void ToSendChan(int fd, std::string msg);
        void serv324(std::string serverName, std::string NameChanel, std::string nickname);
        void serv324SenderOnly(Client &sender, std::string serverName, std::string NameChanel,std::string param,char letter);

};
