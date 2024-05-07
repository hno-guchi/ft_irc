# include <iostream>
# include "./Config.hpp"
# include "../color.hpp"

const std::string	Config::commandList_[] = {
	"PASS",
	"NICK",
	"USER",
	"OPER",
	"QUIT",
	"JOIN",
	"PART",
	"KICK",
	"INVITE",
	"TOPIC",
	"MODE",
	"PRIVMSG",
	"NOTICE",
	"PING",
	"PONG",
	"ERROR",
	""
};

Config::Config() : \
	maxClient_(5), maxChannel_(5), createdDate_(std::time(NULL)), version_("1"), serverName_("ft_irc.42tokyo.net"), password_("password"), userModes_("or"), channelModes_("iklot") {
}

// Config&	operator=(const Config& rhs) {
// 	if (this == &rhs) {
// 		return (*this);
// 	}
// 	this->maxClient_ = rhs.getMaxClient();
// 	this->maxChannel_ = rhs.getMaxChannel();
// 	this->version_ = rhs.getVersion();
// 	this->serverName_ = rhs.getServerName();
// 	this->userModes_ = rhs.getUserModes();
// 	this->channelModes_ = rhs.getChannelModes();
// 	return (*this);
// }

Config::~Config() {}

int	Config::getMaxClient() const {
	return (this->maxClient_);
}

int	Config::getMaxChannel() const {
	return (this->maxChannel_);
}

const std::string&	Config::getVersion() const {
	return (this->version_);
}

const time_t&	Config::getCreatedDate() const {
	return (this->createdDate_);
}

const std::string&	Config::getServerName() const {
	return (this->serverName_);
}

const std::string&	Config::getPassword() const {
	return (this->password_);
}

const std::string&	Config::getUserModes() const {
	return (this->userModes_);
}

const std::string&	Config::getChannelModes() const {
	return (this->channelModes_);
}

const std::string*	Config::getCommandList() const {
	return (this->commandList_);
}

// SETTER
// void	Config::setMaxClient(int maxClient) {
// 	this->maxClient_ = maxClient;
// }
// void	Config::setMaxChannel(int maxChannel) {
// 	this->maxChannel_ = maxChannel;
// }
// void	Config::setVersion(int version) {
// 	this->version_ = version;
// }
// void	Config::setServerName(const std::string& serverName) {
// 	this->serverName_ = serverName;
// }
// void	Config::setUserModes(const std::string& userModes) {
// 	this->userModes_ = userModes;
// }
// void	Config::setChannelModes(const std::string& channelModes) {
// 	this->channelModes_ = channelModes;
// }

void	Config::printData() const {
	std::cout << "[CONFIG INFO] ------------------" << std::endl;
	std::cout << "[maxClient]    : [" << this->maxClient_ << "]" << std::endl;
	std::cout << "[maxChannel]   : [" << this->maxChannel_ << "]" << std::endl;
	std::cout << "[createdDate]  : [" << std::asctime(std::localtime(&(this->createdDate_))) << std::flush;  // << "]" << std::endl;
	std::cout << "[version]      : [" << this->version_ << "]" << std::endl;
	std::cout << "[serverName]   : [" << this->serverName_ << "]" << std::endl;
	std::cout << "[password]     : [" << this->password_ << "]" << std::endl;
	std::cout << "[userModes]    : [" << this->userModes_ << "]" << std::endl;
	std::cout << "[channelModes] : [" << this->channelModes_ << "]" << std::endl;
	std::cout << "[commandList]  : ["<< std::flush;
	for (int i = 0; this->commandList_[i] != ""; i++) {
		std::cout << this->commandList_[i] << std::flush;
		if ((i + 1) % 5 != 0 && this->commandList_[i + 1] != "") {
			std::cout << ", " << std::flush;
		}
		if ((i + 1) % 5 == 0) {
			std::cout << "]" << std::endl;
		}
		if ((i + 1) % 5 == 0 && this->commandList_[i + 1] != "") {
			std::cout << "               : ["<< std::flush;
		}
		if (this->commandList_[i + 1] == "") {
			std::cout << "]" << std::endl;
		}
	}
	std::cout << "--------------------------------\n" << std::endl;
}
