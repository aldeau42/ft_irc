#include "Client.hpp"

Client::Client(){_refuse = false;}
        

const std::string Client::getIpAdd()
{
    return(_IPadd);
} 

bool Client::fullCommand(){
    std::cout << _command << std::endl;
    std::size_t found = _command.find('\n');
    if (found!=std::string::npos)
        return true;
    return false;
}

int Client::GetFd(){
    return this->Fd;
}

std::string Client::GetUsername(){
    return this->_username;
}

std::string Client::GetNickname(){
    return this->_nickname;
}

std::string Client::GetPass(){
    return this->_pass;
}

bool Client::GetReg(){
    return _reg;
}

bool Client::getRefuse(){
    return (_refuse);
}

void Client::addCommand(std::string com){
    _command.append(com);
}

void Client::delCommand(){
    _command.clear();
}

std::string Client::GetCommand(){
    return _command;
}

void Client::setRefuse(bool rep){
    _refuse = rep;
}

void Client::SetUsername(std::string newUserName){
    _username = newUserName;
}

void Client::SetNickname(std::string newNickName){
    _nickname = newNickName;
}

void Client::SetPass(std::string newPass){
    _pass = newPass;
}

void Client::SetFd(int fd){
    Fd = fd;
}

void Client::setIpAdd(std::string ipadd){
    _IPadd = ipadd;
}

void Client::SetReg(bool state) {
    _reg = state;
}

void Client::SetModes(std::string modes){
    _modes = modes;
}

