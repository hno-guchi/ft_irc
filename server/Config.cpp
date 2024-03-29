# include "./Config.hpp"

const std::string	Config::commandList_[] = {
	"PASS",
	"NICK",
	"USER",
	"OPER",
	"QUIT",
	"JOIN",
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
	maxClients_(5), version_(1), serverName_("ft_irc.42tokyo.net"), \
	userModes_("aor"), channelModes_("iklot") {
}

Config::~Config() {}

int					Config::getMaxClients() const {
	return (this->maxClients_);
}

int					Config::getVersion() const {
	return (this->version_);
}

const std::string&	Config::getServerName() const {
	return (this->serverName_);
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
