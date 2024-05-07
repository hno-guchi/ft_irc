#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <string>
# include <sstream>
# include <ctime>

class Config {
 private:
	 const int					maxClient_;
	 const int					maxChannel_;
	 const time_t				createdDate_;
	 const std::string			version_;
	 const std::string			serverName_;
	 const std::string			password_;
	 const std::string			userModes_;
	 const std::string			channelModes_;
	 static const std::string	commandList_[];

 public:
	 Config();
	 ~Config();
	 // TODO(hnoguchi): constを外すなら、コピーアサインメント演算子をオーバーロードする
	 // Config&	operator=(const Config& rhs);
	 // GETTERS
	 int				getMaxClient() const;
	 int				getMaxChannel() const;
	 const time_t&		getCreatedDate() const;
	 const std::string&	getVersion() const;
	 const std::string&	getServerName() const;
	 const std::string&	getPassword() const;
	 const std::string&	getUserModes() const;
	 const std::string&	getChannelModes() const;
	 const std::string*	getCommandList() const;
	 // SETTER
	 // void	setMaxClient(int maxClient);
	 // void	setMaxChannel(int maxChannel);
	 // void	setVersion(int version);
	 // void	setServerName(const std::string& serverName);
	 // void	setPassword(const std::string& password);
	 // void	setUserModes(const std::string& userModes);
	 // void	setChannelModes(const std::string& channelModes);
	 // DEBUG
	 void				printData() const;
};

#endif  // CONFIG_HPP
