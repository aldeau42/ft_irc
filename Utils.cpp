#include "Server.hpp"

std::string intToString(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

std::vector<std::string> Server::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string Server::remove_unprintable(std::string str) {
	for (size_t i = 0; i < str.size(); i++){
		if (str[i] < 32 || str[i] > 126) {
			str = str.substr(0, i);
			break;
		}
	}
	return str;
}

void Server::addChannel(std::string nameChanel) {
	chanelles.push_back(Channel(nameChanel));
}

std::string extractNickname(const std::string& data) {
    std::vector<std::string> tokens;
    std::istringstream stream(data);
    std::string token;

    while (stream >> token) {
        tokens.push_back(token);
    }
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == "NICK" && i + 1 < tokens.size()) {
            return tokens[i + 1];
        }
    }
    return "";
}

bool Server::already_used_nick(std::string nick) {
	for (size_t i=0; i < clients.size(); i++){
		if (nick == clients[i].GetNickname())
			return false;
	}
	return true;
}

void Server::ToSend(int fd, std::string msg) {
    send(fd, msg.c_str(), msg.size(), MSG_NOSIGNAL);
    std::cout << "to send :" << msg << ":end send" << std::endl;
}

void Server::ToSendServer(std::string msg) {
    for (size_t i = 0; i < clients.size(); i++) {
        send(clients[i].GetFd(), msg.c_str(), msg.size(), MSG_NOSIGNAL);
    }
}

bool Server::checkReg(int fd) {
    if (!getClientByFd(fd).GetReg()) {
        ToSend(fd, ERR_NOTREGISTERED(getClientByFd(fd).GetNickname()));
        return false;
    }
    return true;
}

bool Server::checkParams(int fd, const std::vector<std::string>& params, size_t nb) {
    if (params.size() < nb) {
        ToSend(fd, ERR_NEEDMODEPARM(params[0]));
        return false;
    }
    return true;
}

int Server::checkChannel(std::string channel) {
    for (size_t i = 0; i < chanelles.size(); i++) {
        if (chanelles[i].getChannelName() == channel)
            return i;
    }
    return -1;
}

int Server::checkNick(std::string nickname) {
    for (size_t i = 0; i < clients.size(); i++) {   
        std::cout << BLU << clients[i].GetNickname() << WHI << std::endl;
        if (clients[i].GetNickname() == nickname)
            return i;
    }
    return -1;
}

bool Server::checkTopic(Channel& channel) {
    if (channel.getTopic().size() != 0)
        return true;
    return false;
}

bool Server::checkNumber(const std::string& s) {
    if (s.empty())
        return false;
    char* end;
    errno = 0;
    long int value = std::strtol(s.c_str(), &end, 10);
    (void)value;
    if (end == s.c_str() || *end != '\0' || errno == ERANGE) {
        return false;
    }
    return true;
}

std::string Server::onlyParams(const std::vector<std::string>& parts) {
    std::string str = "";
    for (size_t i = 1; i < parts.size(); i++) {
        str += parts[i];
        if (i != parts.size() - 1)
            str += " ";
    }
    return str;
}

std::vector<std::string>    Server::checkCommasCmd(std::string str) {
    std::vector<std::string> prm = split(str, ',');
    return prm;
}