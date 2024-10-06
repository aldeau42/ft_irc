#include "Server.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
/// COMMANDS FOR ALL CLIENTS ///
//////////////////////////////////////////////////////////////////////////////////////////////////

void Server::Ping(int fd, const std::vector<std::string>& parts) {
    std::string PONG;
    if (parts.size() > 2) {
        std::cerr << "error" << std::endl;
        return;
    }
    else if (parts.size() == 2)
        PONG = "PONG " + parts [1] + "\r\n";
    else
        PONG = parts[0] + "\r\n";
    ToSend(fd, PONG);
}

void Server::User(int fd, const std::vector<std::string>& parts) {
    
    Client& client = getClientByFd(fd);
    std::stringstream message;
    std::cout << YEL << "username: " << client.GetUsername() << WHI << std::endl;
    if (!client.GetUsername().empty())
        ToSend(fd, ERR_ALREADYREGISTERED(client.GetNickname()));
    if (!checkParams(fd, parts, 2))
        return;
    message << parts[1];
    client.SetUsername(message.str());
    std::cout << "USER: " << message.str() << " set for client <" << fd << ">" << std::endl;
    connecting(client);
}

void Server::Nick(int fd, const std::vector<std::string>& parts) {
    Client& client = getClientByFd(fd);
    std::stringstream newNick;
    std::stringstream message;
    if  (parts.size() < 1){
        ToSend(fd, ERR_NONICKNAMEGIVEN(client.GetNickname()));
        return;
    }
    newNick << parts[1];
    if (newNick.str() == client.GetNickname()){
        ToSend(fd, ERR_NONICKCHANGE(newNick.str()));
        return;
    }    
    for (size_t i = 0; i < clients.size(); i++){
        if (clients[i].GetNickname() == newNick.str()){
            ToSend(fd, ERR_NICKNAMEINUSE(newNick.str()));
            return;
        }
    }
    if (client.GetNickname().empty())
        message << fd << " changed his nickname to " << newNick.str() << "\r\n";
    else
        message << client.GetNickname() << " changed his nickname to " << newNick.str() << "\r\n";
    client.SetNickname(newNick.str());
    ToSendServer(message.str());
    if (!client.GetReg())
        connecting(client);
    
}

void Server::Pass(int fd, const std::vector<std::string>& parts) {
    Client& client = getClientByFd(fd);
    std::stringstream message;
    std::stringstream newPass;
    if (checkParams (fd, parts, 2) == false)
        return;
    newPass << parts[1];
    if (_passWord != newPass.str()) {
        ToSend(fd, ERR_PASSWDMISMATCH(client.GetNickname()));
        return;
    }
    client.SetPass(newPass.str());
    std::cout << client.GetPass() << std::endl;
    connecting(client);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
/// COMMANDS FOR REGISTERED CLIENTS ///
//////////////////////////////////////////////////////////////////////////////////////////////////

void Server::Msg(int fd, const std::vector<std::string>& parts) {
    if (checkParams (fd, parts, 3) == false)
        return;
    if (checkReg (fd) == false)
        return;
    std::vector<std::string> names = split(parts[1], ',');
    Client& cli = getClientByFd(fd);
    std::string message = ":" + cli.GetNickname() + "!" + cli.GetUsername() + "@localhost PRIVMSG " ;
    message.append(parts[1] + " ");
    if (parts[2][0] != ':')
        message.append(":");
    for (size_t i = 2; i < parts.size(); i++){
        message.append(parts[i]);
        if (i + 1 < parts.size())
            message.append(" ");
        std::cout << RED << message << WHI << std::endl;
    }
    message.append("\r\n");
    for (size_t i = 0; i < names.size(); i++) {
        if (names[i][0] == '#') {
            std::string nameChan = names[i].substr(1);
            int indexChan = checkChannel(nameChan);
            if (indexChan == -1) {
                ToSend(fd, ERR_NOSUCHCHANNEL(names[i]));
                return;
            }
            Channel chan = chanelles[indexChan];
            chan.ToSendChan(fd, message);
        }
        else {
            int indexNick = checkNick(names[i]);
            if (indexNick == -1) {
                ToSend(fd, ERR_NOSUCHNICK(names[1]));
                return;
            }
            ToSend(clients[indexNick].GetFd(), message);
        }
    }
}

void Server::sendJoinMessages(int fd, std::string channel, Client& sender, int isChannel) {
    std::string senderNickname = sender.GetNickname();
    std::string servJOIN = ":" + senderNickname + "!" + sender.GetUsername() + "@localhost JOIN :#" + channel + "\r\n";
    std::string servMODE = ":" + serverName + " MODE #" + channel + " " + chanelles[isChannel].getModes() + "\r\n";
    ToSend(fd, servJOIN.c_str());
    chanelles[isChannel].ToSendChan(fd, servJOIN.c_str());
    ToSend(fd, servMODE.c_str());
    if (checkTopic(chanelles[isChannel]) == true) {
        ToSend(fd, serv332(serverName, channel, senderNickname, chanelles[isChannel].getTopic()));
        ToSend(fd, serv333(serverName, channel, senderNickname, chanelles[isChannel].getTopicSetter()));
    }
    std::vector<std::string> listName = chanelles[isChannel].clientsList();
    for (size_t i = 0; i < listName.size(); i++)
    {
        if (chanelles[isChannel].checkOperator(listName[i]) == true)
            ToSend(fd, serv353(serverName, senderNickname, listName[i], channel, 1));
        else 
            ToSend(fd, serv353(serverName, senderNickname, listName[i], channel, 0));
    }
    ToSend(fd, serv366(serverName, senderNickname, channel));
    
}

bool Server::checkKey(int isChannel, const std::vector<std::string>& parts) {
    if (parts.size() != 2) {
        return false;
    }   
    if (parts[1] != chanelles[isChannel].getK()) {
        if (chanelles[isChannel].getChannelClientsCount() == 0)
            chanelles.erase(chanelles.begin() + isChannel);
        return false;
    }
    return true;
}

void Server::Join2(int fd, std::string params) { 
    std::vector<std::string> parts = split(params, ' ');
    std::string channel = parts[0];
    if (channel[0] == '#')
        channel = channel.substr(1);
    
    Client sender = getClientByFd(fd);
    Channel c(channel);
    int isChannel = checkChannel(channel);
    if (isChannel < 0) {
        c.addClientToChannel(sender);
        c.setNeedKey(false);
        c.setI(false);
        c.setT(false);
        c.addOperatorToChannel(sender);
        chanelles.push_back(c);
        isChannel = checkChannel(channel);
        sendJoinMessages(fd, channel, sender, isChannel);
        return;
    } else {
        bool OnChannel = chanelles[isChannel].checkClientChannel(sender.GetNickname());
	 	if (OnChannel == true) {
		        ToSend(fd, ERR_USERONCHANNEL(sender.GetNickname(), channel));
		        return;
		}
        bool inv = chanelles[isChannel].isInvited(sender.GetNickname());
		if (chanelles[isChannel].getI() && !inv) {
		        ToSend(fd, ERR_INVITEONLYCHAN(sender.GetNickname(), channel));
		        return;
	    }	    
	    if (chanelles[isChannel].getL() == chanelles[isChannel].getChannelClientsCount() ) {
			ToSend(fd, serv471(serverName, channel, sender.GetNickname()));
	        return ;
	    }
        if (chanelles[isChannel].getNeedKey() == true) {
            if (checkKey(isChannel, parts) == false) {
                std::string serv475 = ":" + serverName + " 475 " + sender.GetNickname() + " #" + channel + " :Cannot join channel (+k) - bad key\r\n";
                ToSend(sender.GetFd(), serv475);
                return;
            }
        }
    }
    chanelles[isChannel].addClientToChannel(sender);
    chanelles[isChannel].removeInvitedFromList(sender.GetNickname());
    sendJoinMessages(fd, channel, sender, isChannel);
    return;
}
 
void Server::Join(int fd, const std::vector<std::string>& parts) {
    if (checkParams (fd, parts, 2) == false)
        return;
    if (checkReg (fd) == false)
        return;
    std::string params = onlyParams(parts);
    std::vector<std::string> prm;
    if (params.find(',') != std::string::npos)
        prm = checkCommasCmd(params);
    if (prm.size() == 0)
        Join2(fd, params);
    else {
        for (size_t i = 0; i < prm.size(); i++)
            Join2(fd, prm[i]);
    } 
}

void Server::Kick(int fd, const std::vector<std::string>& parts) {
    if (checkParams (fd, parts, 3) == false)
        return;
    if (checkReg (fd) == false)
        return;

    std::string channel = parts[1];
    if (channel[0] == '#')
        channel = channel.substr(1);
    std::string nickname = parts[2];
    std::string comments;
    if (parts.size() > 3) {
        for (size_t i = 3; i < parts.size(); i++)
            comments.append(parts[i] + " ");
    }

    Client sender = getClientByFd(fd);
    std::string senderNick = sender.GetNickname();
    std::string senderUser = sender.GetUsername();

    int isChannel = checkChannel(channel);
    if (isChannel < 0) {
        ToSend(fd, ERR_NOSUCHCHANNEL(channel));
        return ;
    }

    int isNickname = checkNick(nickname);
    if (isNickname < 0) {
        ToSend(fd, ERR_NOSUCHNICK(nickname));
        ToSend(fd, serv441(serverName, channel, senderNick, nickname));
        return ;
    }

    bool isOnChannel = chanelles[isChannel].checkClientChannel(nickname);
    if (isOnChannel == false) {
        ToSend(fd, serv441(serverName, channel, senderNick, nickname));
        return;
    }

    bool isSenderOnChannel = chanelles[isChannel].checkClientChannel(senderNick);
    if (isSenderOnChannel == false){
        ToSend(fd, serv442(serverName, channel, senderNick));
        return;
    }

    bool isOperator = chanelles[isChannel].checkOperator(senderNick);   
    if (isOperator == false) {
        ToSend(fd, serv482(serverName, channel, senderNick));
        return;
    }

    Client cli = getClientByNickname(nickname);
    if (isOnChannel == true && isSenderOnChannel == true && isOperator == true) {
        if (comments.empty()) {
            ToSend(fd, KICKING(senderNick, senderUser, channel, nickname));
            chanelles[isChannel].ToSendChan(fd, KICKING(senderNick, senderUser, channel, nickname));
        } else {
            ToSend(fd, KICKING_COM(senderNick, senderUser, channel, nickname, comments));
            chanelles[isChannel].ToSendChan(fd, KICKING_COM(senderNick, senderUser, channel, nickname, comments));
        }
        chanelles[isChannel].removeClientChannelByFd(cli.GetFd());
    }
}

void Server::Invite(int fd, const std::vector<std::string>& parts) {
    if (checkParams (fd, parts, 3) == false)
        return;
    if (checkReg (fd) == false)
        return;

    std::string nickname = parts[1];
    std::string channel = parts[2];
    if (channel[0] == '#')
        channel = channel.substr(1);
    Client sender = getClientByFd(fd);
    int isChannel = checkChannel(channel);
    int isNickname = checkNick(nickname);

    if (isNickname < 0) {
        ToSend(fd, ERR_NOSUCHNICK(nickname));
        return ;
    }
    bool isOnChannel = chanelles[isChannel].checkClientChannel(clients[isNickname].GetNickname());
    if (isOnChannel == true) {
        ToSend(fd, ERR_USERONCHANNEL(clients[isNickname].GetNickname(), chanelles[isChannel].getChannelName()));
        return;
    }
    if (isChannel > 0){
        if(chanelles[isChannel].getI() && !chanelles[isChannel].checkOperator(sender.GetNickname())) {
            ToSend(fd, ERR_CHANOPRIVSNEEDED(sender.GetNickname(), chanelles[isChannel].getChannelName()));
            return ;
        }
        chanelles.push_back(nickname);
    }
    chanelles[isChannel].addInvitedToList(clients[isNickname]);
    chanelles[isChannel].invitedList();
    ToSend(clients[isNickname].GetFd(), Serv341(serverName, nickname, sender.GetUsername(), channel));
}

void Server::Topic(int fd, const std::vector<std::string>& parts) {
    if (checkParams (fd, parts, 2) == false)
        return;
    if (checkReg (fd) == false)
        return;

    std::string channel = parts[1];
    if (channel[0] == '#')
        channel = channel.substr(1);
    std::string topic = "";
    if (parts.size() > 2) {
        for (size_t i = 2; i < parts.size(); i++)
            topic.append(parts[i] + " ");
    }

    int isChannel = checkChannel(channel);
    if (isChannel < 0) {
        ToSend(fd, ERR_NOSUCHCHANNEL(channel));
        return ;
    }

    Client sender = getClientByFd(fd);
    std::string nickname = sender.GetNickname();
    bool isOnChannel = chanelles[isChannel].checkClientChannel(nickname);
    bool isOperator = chanelles[isChannel].checkOperator(nickname);
    bool hasTopic = checkTopic(chanelles[isChannel]);

    if (parts.size() < 3) {
        if (hasTopic == true) {
            ToSend(sender.GetFd(), serv332(serverName, channel, nickname, chanelles[isChannel].getTopic()));
            ToSend(sender.GetFd(), serv333(serverName, channel, nickname, chanelles[isChannel].getTopicSetter()));
        } else
            ToSend(sender.GetFd(), serv331(serverName, channel, nickname));
        return ;
    } else {
        if (chanelles[isChannel].getT() == false)
            return;
        if (isOnChannel == false) {
            ToSend(sender.GetFd(), serv442(serverName, channel, nickname));
            return;
        }
        if (isOperator == false && chanelles[isChannel].getT() == true) {
            ToSend(sender.GetFd(), serv482(serverName, channel, nickname));
            return;
        }
    chanelles[isChannel].setTopic(topic.substr(1), nickname);
    std::string username = sender.GetUsername();
    std::string msg;
    msg = ":" + nickname + "!" + username + "@localhost TOPIC #" + channel + " :" + topic.substr(1) + "\r\n";
    chanelles[isChannel].ToSendChan(fd, msg);
    ToSend(sender.GetFd(), serv332(serverName, channel, nickname, topic.substr(1)));
    }
}

void Server::delChannel() 
{
    for (size_t i = 0; i < chanelles.size(); i++)
    {
        if (chanelles[i].getChannelClients().size() <= 0)
            chanelles.erase( chanelles.begin()+i, chanelles.begin()+i+1);
    }
}

void Server::Quit(int fd, const std::vector<std::string>& parts)  
{
    Client sender = getClientByFd(fd);
    std::string msg;

    std::string nickname = sender.GetNickname();;
    std::string username = sender.GetUsername();
    std::string host = "127.0.0.1";


    if (parts.size() == 2)
    {
        std::string message = parts[1];
        msg = "ERROR :Closing Link: " + nickname + "[" + username + "@" + host + "] (Quit: " + message + ")\r\n";
    }
    else
        msg = "ERROR :Closing Link: " + nickname + "[" + username + "@" + host + "] (Quit)\r\n";
    ToSend(sender.GetFd(), msg);

    for (size_t i = 0; chanelles.size() > i; i++)
    {
        for (size_t x = 0; chanelles[i].getChannelClients().size() > x; x++)
        {
            if (chanelles[i].getChannelClients()[x].GetNickname() == sender.GetNickname())
                chanelles[i].eraseCli(x);
        }
    }
    delChannel();
}
