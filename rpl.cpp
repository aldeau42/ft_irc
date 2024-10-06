
#include "rpl.hpp"

std::string ERR_NOSUCHNICK(std::string nickname) 			             {std::string str = (": 401 " + nickname + " :No such nick/channel\r\n"); return str;}
std::string ERR_NOSUCHCHANNEL(std::string chan) 			             {std::string str = (": 403 " + chan     + " :No such channel\r\n"); return str;}
std::string ERR_NICKNAMEINUSE(std::string nickname) 			         {std::string str = (": 433 " + nickname + " " + nickname + " :Nickname already in use\r\n"); return str;}
std::string ERR_NONICKNAMEGIVEN(std::string nickname) 			         {std::string str = (": 431 " + nickname + " :No nickname given\r\n"); return str;}
std::string ERR_NOTONCHANNEL(std::string nickname) 			             {std::string str = (": 442 " + nickname + " :User not already on the channel\r\n"); return str;}
std::string ERR_USERONCHANNEL(std::string nickname, std::string channel) {std::string str = (": 443 " + nickname + " " + channel + " :User Already on channel\r\n"); return str;}
std::string ERR_NONICKCHANGE(std::string nickname) 			             {std::string str = (": 447 " + nickname + " :Same nickname\r\n"); return str;}
std::string ERR_NOTREGISTERED(std::string nickname) 			         {std::string str = (": 451 " + nickname + " :You have not registered!\r\n"); return str;}
std::string ERR_NEEDMODEPARM(std::string command) 			             {std::string str = (": 461 " + command  + " :Not enough parameters\r\n"); return str;}
std::string ERR_ALREADYREGISTERED(std::string nickname) 	             {std::string str = (": 462 " + nickname + " :You are already registered!\r\n"); return str;}
std::string ERR_PASSWDMISMATCH(std::string nickname) 			         {std::string str = (": 464 " + nickname + " :Password incorrect\r\n"); return str;}
std::string ERR_CHANOPRIVSNEEDED(std::string nickname, std::string chan) {std::string str = (": 482 " + nickname + " " + chan + " :You're not channel operator\r\n"); return str;}
std::string ERR_INVITEONLYCHAN(std::string nickname, std::string chan) 			             {std::string str = (": 473 " + nickname + " " +  chan     + " :Cannot join channel (+i)\r\n"); return str;}

std::string KICKING(std::string nickname, std::string username, std::string channel, std::string kickedname){
    std::string str = (":" + nickname + "!" + username + "@localhost KICK #" + channel + " " + kickedname + " :"  + nickname +  "\r\n");
    return str;
}

std::string KICKING_COM(std::string nickname, std::string username, std::string channel, std::string kickedname, std::string comment){
    std::string str = (":" + nickname + "!" + username + "@localhost KICK " + " #" + channel + " " + kickedname + " :"  + comment +  "\r\n");
    return str;
}

std::string serv329(std::string serverName, std::string NameChanel, std::string nickname)
{
    time_t seconds;
    seconds = time (NULL);
    std::stringstream message;
    message << ":" << serverName << " 329 " << nickname << " #" << NameChanel << " " << seconds << "\r\n";
    return message.str();
}

std::string serv331(std::string serverName, std::string NameChanel, std::string nickname) {
    std::string str = (":" + serverName + " 331 " + nickname + " #" + NameChanel + " :No topic is set\r\n");
    return str;
}

std::string serv332(std::string serverName, std::string NameChanel, std::string nickname, std::string topic){
    std::string str = (":" + serverName + " 332 " + nickname + " #" + NameChanel + " :" + topic + "\r\n");
    return str;
}

std::string serv333(std::string serverName, std::string NameChanel, std::string nickname, std::string topicSetter){
    time_t seconds;
    seconds = time (NULL);
    std::stringstream message;
    message << ":" << serverName << " 333 " << nickname << " #" << NameChanel << " " << topicSetter << " " << seconds << "\r\n"; 
    return message.str();
}

std::string Serv341(std::string serverName, std::string nickname, std::string username, std::string channel){
    std::string str = (":" + serverName + " 341 " +  nickname + " " + username + " #" + channel + "\r\n");
    return str;
}

std::string Serv342(std::string serverName, std::string nickname, std::string username, std::string channel){
    std::string str = (":" + serverName + " 342 " +  nickname + " " + username + " #" + channel + " :You're not on that channel\r\n");
    return str;
}

std::string serv353(std::string serverName, std::string sender_nickname, std::string nickname, std::string NameChanel, int isOp){
    std::string str;
    if (isOp == 1)
        str = (":" + serverName + " 353 " + sender_nickname + " = #" + NameChanel + " :@" + nickname + "\r\n");
    else
        str = (":" + serverName + " 353 " + sender_nickname + " = #" + NameChanel + " :" + nickname + "\r\n");
    return str;
}

std::string serv366(std::string serverName, std::string nickname, std::string NameChanel){
    std::string str = (":" + serverName + " 366 " + nickname + " #" + NameChanel + " :End of /NAMES list" + "\r\n");
    return str;
}

std::string serv441(std::string serverName, std::string NameChanel, std::string nickname, std::string user){
    std::string str = (":" + serverName + " 441 " + nickname + " " + user + " #" + NameChanel + " :They aren't on that channel\r\n");
    return str;
}

std::string serv442(std::string serverName, std::string NameChanel, std::string nickname){
    std::string str = (":" + serverName + " 442 " + nickname + " #" + NameChanel + " :You're not on that channel\r\n");
    return str;
}

std::string serv471(std::string serverName, std::string NameChanel, std::string nickname){
    std::string str = (":" + serverName + " 471 " + nickname + " #" + NameChanel + " :Cannot join channel (+l)\r\n");
    return str;
}

std::string serv473(std::string serverName, std::string NameChanel, std::string nickname){
    std::string str = (":" + serverName + " 473 " + nickname + " #" + NameChanel + " :Cannot join channel (+i)\r\n");
    return str;
}

std::string serv482(std::string serverName, std::string NameChanel, std::string nickname){
    std::string str = (":" + serverName + " 482 " + nickname + " #" + NameChanel + " :You're not channel operator\r\n");
    return str;
}
