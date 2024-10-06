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

class Client {
    private:
        int Fd;
        std::string _IPadd;
        std::string _nickname;
        std::string _username;
        std::string _pass;
        std::string _modes;
        std::string _command;
        bool        _reg;
        bool        _refuse;

    public:
        Client();
        
        int         GetFd();
        std::string GetUsername();
        std::string GetNickname();
        std::string GetPass();
        std::string GetCommand();
		bool        GetReg();
        bool        getRefuse();
        bool        fullCommand();
        void        addCommand(std::string com);
        void        delCommand();
        void setRefuse(bool rep);
        void SetUsername(std::string newUserName);
        void SetNickname(std::string newNickName);
        void SetPass(std::string newPass);
        void SetFd(int fd);
        void setIpAdd(std::string ipadd);
		void SetReg(bool state);
		void SetModes(std::string modes);
        const std::string getIpAdd();
};