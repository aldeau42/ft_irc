#include "Channel.hpp"


Channel::Channel(std::string name_arg) {
    _channelName = name_arg;
    _limit = 0;
}

Channel::~Channel() {}


void Channel::setChannelName(std::string newName) {
    _channelName = newName;
}

void Channel::setTopic(std::string newTopic, std::string nickname) {
    _topic = newTopic;
     _topicSetter = nickname;
}

void Channel::setNeedKey(bool needKey) {
    _needKey = needKey;
}

std::string Channel::getChannelName() {
    return (_channelName);
}

size_t      Channel::getChannelClientsCount() {
    return _channelClients.size();
}

bool        Channel::getNeedKey() {
    return _needKey;
}

std::string Channel::getTopic() {
    return _topic;
}

std::string Channel::getTopicSetter() {
    return _topicSetter;
}

std::vector<Client> Channel::getChannelClients()
{
    return _channelClients;
}

void Channel::eraseCli(size_t x)
{
    _channelClients.erase(_channelClients.begin() + x, _channelClients.begin() + x + 1);
}

void Channel::setI(bool i) {
    _i = i;
}

void Channel::setT(bool t) {
    _t = t;
}

void Channel::setK(std::string key) {
    _key = key;
}

void Channel::setL(size_t limit) {
    _limit = limit;
}

bool Channel::getI() {
    return _i;
}

bool Channel::getT() {
    return _t;
}

std::string Channel::getK() {
    return _key;
}

size_t Channel::getL() {
    return _limit;
}



std::string Channel::getModes() {
    std::string modes = "";
    if (_i == true)
        modes += "i";
    if (_t == true)
        modes += "t";
    if (_needKey == true)
        modes += "k";
    if (_limit != 0)
        modes += "l";
    if (!modes.empty())
    	modes.insert(0, 1, '+'); 
    return modes;
}

void Channel::ToSendChan(int fd, std::string msg) {
    for (size_t i = 0; i < _channelClients.size(); i++) {
        if (_channelClients[i].GetFd() != fd)
           send(_channelClients[i].GetFd(), msg.c_str(), msg.size(), MSG_NOSIGNAL);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////
/// CLIENTS ///
//////////////////////////////////////////////////////////////////////////////////////////////////

void Channel::addClientToChannel(Client& cli) {
    _channelClients.push_back(cli);
}

void Channel::removeClientChannelByFd(int fd) {
    for (size_t i = 0; i < _channelClients.size(); i++) {
        if (_channelClients[i].GetFd() == fd)
            _channelClients.erase(_channelClients.begin() + i);
    }
}

bool Channel::checkClientChannel(std::string nickname) {
    for (size_t i = 0; i < _channelClients.size(); i++) {
        if (_channelClients[i].GetNickname() == nickname)
            return true;
    }
    return false;
}

std::vector<std::string> Channel::clientsList() {
    std::vector<std::string> list;
    for (size_t i = 0; i < _channelClients.size(); i++)
        list.push_back(_channelClients[i].GetNickname());
    return list;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
/// OPERATORS ///
//////////////////////////////////////////////////////////////////////////////////////////////////

void Channel::addOperatorToChannel(Client& cli) {
    _operatorNicknames.push_back(cli);
}

void Channel::removeOperatorByFd(int fd) {
    for (size_t i = 0; i < _operatorNicknames.size(); i++) {
        if (_operatorNicknames[i].GetFd() == fd)
            _operatorNicknames.erase(_operatorNicknames.begin() + i);
    }
}

bool Channel::checkOperator(std::string nickname) {
    for (size_t i = 0; i < _operatorNicknames.size(); i++) {
        if (_operatorNicknames[i].GetNickname() == nickname)
            return true;
    }
    return false;
}

bool Channel::isOperator() {
    if (_operatorNicknames.size() != 0)
            return true;
    return false;
}

void Channel::operatorsList() {
    for (size_t i = 0; i < _operatorNicknames.size(); i++) {
        std:: cout << GRE << _operatorNicknames[i].GetNickname() << WHI << std::endl;
    }
} 

//////////////////////////////////////////////////////////////////////////////////////////////////
/// INVITED ///
//////////////////////////////////////////////////////////////////////////////////////////////////

void Channel::addInvitedToList(Client& cli) {
    _invitedList.push_back(cli);
}

void Channel::removeInvitedFromList(std::string nickname) {
    for(size_t i = 0; i < _invitedList.size(); i++) {
        if (nickname == _invitedList[i].GetNickname())
            _invitedList.erase(_invitedList.begin() + i);
    }
}

bool Channel::isInvited(std::string nickname) {
    for(size_t i = 0; i < _invitedList.size(); i++) {
        if (nickname == _invitedList[i].GetNickname())
            return true;
    }
    return false;
}

void Channel::invitedList() {
    for (size_t i = 0; i < _invitedList.size(); i++) {
        std:: cout << GRE << _invitedList[i].GetNickname() << WHI << std::endl;
    }
}

void Channel::serv324(std::string serverName, std::string NameChanel, std::string nickname) {
    std::string str324;

    for (size_t i = 0; _channelClients.size() > i; i++) {
        for (size_t u = 0; _operatorNicknames.size() > u; u++) {
            if (_operatorNicknames[u].GetNickname() == _channelClients[i].GetNickname())
                str324 = ":" + serverName + " 324 " + nickname + " #" + NameChanel + " " + getModes() + "o\r\n";
            else
                str324 = ":" + serverName + " 324 " + nickname + " #" + NameChanel + " " + getModes() + "\r\n";
            send(_channelClients[i].GetFd(), str324.c_str(), str324.size(), MSG_NOSIGNAL);
        }
    }
}

