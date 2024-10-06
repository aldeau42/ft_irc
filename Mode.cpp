#include "Server.hpp"

void Server::Mode(int fd, const std::vector<std::string>& parts) { 
    if (checkParams (fd, parts, 2) == false)
        return;
    if (parts[1][0] != '#')
        return;
    if (checkReg (fd) == false)
        return;

    bool add = false;
    std::string channel = parts[1].substr(1);
    if (channel[0] == '#')
        channel = channel.substr(1);
    
    int isChannel = checkChannel(channel);
    if (isChannel < 0) {
        ToSend(fd, ERR_NOSUCHCHANNEL(channel));
        return ;
    }

    Client sender = getClientByFd(fd);
    bool isOperator = chanelles[isChannel].checkOperator(sender.GetNickname()); 
    if (parts.size() == 2 && chanelles[isChannel].checkClientChannel(sender.GetNickname()) == true) {
        std::string serv324 = ":" + serverName + " 324 " + sender.GetNickname() + " #" + channel + " " + chanelles[isChannel].getModes();
        if (isOperator == true)
            serv324 += "o";
        serv324 += "\r\n";
        ToSend(sender.GetFd(), serv324.c_str());
        return;
    }

    std::string modes = parts[2];
    if (parts[2][0] != '+' && parts[2][0] != '-')
        return ;
    if (parts[2][0] == '+')
        add = true;

    if (modes.find('o') != std::string::npos)
        isOperator = ModeO(sender, isChannel, isOperator, add, parts);
    if (modes.find('i') != std::string::npos && isOperator == true)
        ModeI(sender, isChannel, isOperator, add);
    if (modes.find('l') != std::string::npos && isOperator == true)
        ModeL(sender, isChannel, isOperator, add, parts);
    if (modes.find('k') != std::string::npos && isOperator == true)
        ModeK(sender, isChannel, isOperator, add, parts);
    if (modes.find('t') != std::string::npos && isOperator == true)
        ModeT(sender, isChannel, isOperator, add);

    chanelles[isChannel].serv324(serverName, chanelles[isChannel].getChannelName(), sender.GetNickname());
    ToSend(sender.GetFd(), serv329(serverName,chanelles[isChannel].getChannelName(), sender.GetNickname()));
}

void Server::ModeI(Client& sender, int isChannel, bool isOperator, bool add) {
    std::string str = ":" + sender.GetNickname() + "!" + sender.GetUsername() + "@localhost MODE #" + chanelles[isChannel].getChannelName();
    if (add == true && isOperator == true) {
        chanelles[isChannel].setI(true);
        str += " +i\r\n";
        chanelles[isChannel].ToSendChan(sender.GetFd() , str);
        ToSend(sender.GetFd() ,str);
    }
    else if (add == false && isOperator == true) {
        chanelles[isChannel].setI(false);
        str += " -i\r\n";
        chanelles[isChannel].ToSendChan(sender.GetFd() , str);
        ToSend(sender.GetFd(),str);
    }
    else if (isOperator == false) {
        ToSend(sender.GetFd(), serv482(serverName,chanelles[isChannel].getChannelName(), sender.GetNickname()));
        return ;
    }
}

void Server::ModeT(Client& sender, int isChannel, bool isOperator, bool add) {
    std::string str = ":" + sender.GetNickname() + "!" + sender.GetUsername() + "@localhost MODE #" + chanelles[isChannel].getChannelName();
    if (add == true && isOperator == true) {
        chanelles[isChannel].setT(true);	
        str += " +t\r\n";
        chanelles[isChannel].ToSendChan(sender.GetFd() , str);
        ToSend(sender.GetFd() ,str);
    }
    else if (add == false && isOperator == true) {
        chanelles[isChannel].setT(false);
        str += " -t\r\n";
        chanelles[isChannel].ToSendChan(sender.GetFd() , str);
        ToSend(sender.GetFd() ,str);
    }
    else if (isOperator == false) {
        ToSend(sender.GetFd(), serv482(serverName,chanelles[isChannel].getChannelName(), sender.GetNickname()));
        return ;
    }
}

void Server::ModeK(Client& sender, int isChannel, bool isOperator, bool add, const std::vector<std::string>& parts) {
    std::string optkey = "";
    bool isOnChannel = false;
    bool isNum = false;
    if (parts.size() > 3) {
        for (size_t i = 3; i < parts.size(); i++) {
            isNum = checkNumber(parts[i]);
            if (isNum == false) {
                isOnChannel = chanelles[isChannel].checkClientChannel(parts[i]);
                if (isOnChannel == false) {
                    optkey = parts[i];
                    break ;
                }
            }
        }
    }
    std::string str = ":" + sender.GetNickname() + "!" + sender.GetUsername() + "@localhost MODE #" + chanelles[isChannel].getChannelName();
	if ((add == false && isOperator == true) || optkey == "") {
        chanelles[isChannel].setK("");
        chanelles[isChannel].setNeedKey(false);
        
        str += " -k\r\n";
        chanelles[isChannel].ToSendChan(sender.GetFd() , str);
        ToSend(sender.GetFd() ,str);
    } 
    else if (add == true && isOperator == true) {
        chanelles[isChannel].setK(optkey);
        chanelles[isChannel].setNeedKey(true);

        str = str + " +k " + optkey + "\r\n";
        chanelles[isChannel].ToSendChan(sender.GetFd() , str);
        ToSend(sender.GetFd() ,str);
    }
    else if (isOperator == false) {
        ToSend(sender.GetFd(), serv482(serverName,chanelles[isChannel].getChannelName(), sender.GetNickname()));
        return;
    }
}

bool Server::ModeO(Client& sender, int isChannel, bool isOperator, bool add, const std::vector<std::string>& parts) {
    std::string optnick;
    if (parts.size() > 3) {
        bool isOnChannel = false;
        for (size_t i = 2; i < parts.size(); i++){
            isOnChannel = chanelles[isChannel].checkClientChannel(parts[i]);
            if (isOnChannel == true) {
                optnick = parts[i];
                break ;
            }
        }
    }
    if (optnick.empty())
        optnick = sender.GetNickname();
    
    Client& cli = getClientByNickname(optnick);
    std::string str = ":" + serverName + " MODE #" + chanelles[isChannel].getChannelName() + " +o @" + optnick + "\r\n";
    if (add == true && isOperator == false && chanelles[isChannel].isOperator() == false)
    {
        chanelles[isChannel].ToSendChan(sender.GetFd(), str);
        ToSend(sender.GetFd(), str);
        chanelles[isChannel].addOperatorToChannel(cli);
    }
    else if (add == true && isOperator == true)
    {
        chanelles[isChannel].ToSendChan(sender.GetFd(), str);
        ToSend(sender.GetFd(), str);
        chanelles[isChannel].addOperatorToChannel(cli);
    }
    else if (add == false && isOperator == true) {
        str = ":" + serverName + " MODE #" + chanelles[isChannel].getChannelName() + " -o @" + optnick + "\r\n";
        chanelles[isChannel].ToSendChan(sender.GetFd(), str);
        ToSend(sender.GetFd(), str);
        chanelles[isChannel].removeOperatorByFd(cli.GetFd());
        return false;
    }
    else if (isOperator == false) {
        ToSend(sender.GetFd(), serv482(serverName,chanelles[isChannel].getChannelName(), sender.GetNickname()));
        return false;
    }
    return true;
}

void Server::ModeL(Client& sender, int isChannel, bool isOperator, bool add, const std::vector<std::string>& parts) {
    bool isNum = false;
    size_t optnum = 0;
    for (size_t i = 2; i < parts.size(); i++) {
        isNum = checkNumber(parts[i]);
        if (isNum == true) {
            optnum = atoi(parts[i].c_str());
            break ;
        }
    }
    if (add == true && (isNum == false || optnum == 0))
        return;
    
    std::stringstream str;
    str << ":" << sender.GetNickname() << "!" << sender.GetUsername() << "@localhost MODE #" << chanelles[isChannel].getChannelName();
    if (add == true && isOperator == true) {
        chanelles[isChannel].setL(optnum);
        str << " +l " << optnum << "\r\n";

        chanelles[isChannel].ToSendChan(sender.GetFd() , str.str());
        ToSend(sender.GetFd(), str.str());
    }
    else if (add == false && isOperator == true) {
        chanelles[isChannel].setL(0);
        str << " -l\r\n";
        chanelles[isChannel].ToSendChan(sender.GetFd() , str.str());
        ToSend(sender.GetFd(), str.str());
    }
    else if (optnum < chanelles[isChannel].getChannelClientsCount()){
        std::cout << "The limit must be higher than the clients number.\nHere there are " << chanelles[isChannel].getChannelClientsCount() 
        << " clients.\n Or kick some clients to match your limit." << std::endl;
        return;
    }
    else if (isOperator == false) {
        ToSend(sender.GetFd(), serv482(serverName,chanelles[isChannel].getChannelName(), sender.GetNickname()));
        return ;
    }
}