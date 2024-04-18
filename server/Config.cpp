# include "./Config.hpp"

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
	maxClient_(5), maxChannel_(5), version_(1), createdData_(std::time(NULL)), serverName_("ft_irc.42tokyo.net"), password_("password"), userModes_("or"), channelModes_("iklot") {
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

int	Config::getVersion() const {
	return (this->version_);
}

const time_t&	Config::getCreatedData() const {
	return (this->createdData_);
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

