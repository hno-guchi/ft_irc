#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <string>

class Config {
 private:
	 const int					maxClients_;
	 const int					version_;
	 const std::string			serverName_;
	 const std::string			userModes_;
	 const std::string			channelModes_;
	 static const std::string	commandList_[];

 public:
	 Config();
	 ~Config();
	 // GETTERS
	 int				getMaxClients() const;
	 int				getVersion() const;
	 const std::string&	getServerName() const;
	 const std::string&	getUserModes() const;
	 const std::string&	getChannelModes() const;
	 const std::string*	getCommandList() const;
};

#endif  // CONFIG_HPP
