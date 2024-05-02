#ifndef REPLY_HPP
# define REPLY_HPP

#include <string>
#include <ctime>
#include "../user/User.hpp"
#include "../server/Info.hpp"
#include "../parser/Parser.hpp"

// TODO(hnoguchi): welcome messageで使用するものは分ける？
enum kCmdReplyNum {
	kRPL_WELCOME			= 1,
	kRPL_YOURHOST			= 2,
	kRPL_CREATED			= 3,
	kRPL_MYINFO				= 4,
	kRPL_AWAY				= 301,
	kRPL_UMODEIS			= 221,
	kRPL_CHANNELMODEIS		= 324,
	kRPL_UNIQOPIS			= 325,
	kRPL_NOTOPIC			= 331,
	kRPL_TOPIC				= 332,
	kRPL_INVITING			= 341,
	kRPL_INVITELIST			= 346,
	kRPL_ENDOFINVITELIST	= 347,
	kRPL_EXCEPTLIST			= 348,
	kRPL_ENDOFEXCEPTLIST	= 349,
	kRPL_BANLIST			= 367,
	kRPL_ENDOFBANLIST		= 368,
	kRPL_YOUREOPER			= 381
};

enum kErrReplyNum {
	kERR_NOSUCHNICK			= 401,
	kERR_NOSUCHSERVER		= 402,
	kERR_NOSUCHCHANNEL		= 403,
	kERR_CANNOTSENDTOCHAN	= 404,
	kERR_TOOMANYCHANNELS	= 405,
	kERR_TOOMANYTARGETS		= 407,
	kERR_NOORIGIN			= 409,
	kERR_NORECIPIENT		= 411,
	kERR_NOTEXTTOSEND		= 412,
	kERR_NOTOPLEVEL			= 413,
	kERR_WILDTOPLEVEL		= 414,
	kERR_UNKNOWNCOMMAND		= 421,
	kERR_NONICKNAMEGIVEN	= 431,
	kERR_ERRONEUSNICKNAME	= 432,
	kERR_NICKNAMEINUSE		= 433,
	kERR_NICKCOLLISION		= 436,
	kERR_UNAVAILRESOURCE	= 437,
	kERR_USERNOTINCHANNEL	= 441,
	kERR_NOTONCHANNEL		= 442,
	kERR_USERONCHANNEL		= 443,
	kERR_NOTREGISTERED		= 451,
	kERR_NEEDMOREPARAMS		= 461,
	kERR_ALREADYREGISTRED	= 462,
	kERR_PASSWDMISMATCH		= 464,
	kERR_KEYSET				= 467,
	kERR_CHANNELISFULL		= 471,
	kERR_UNKNOWNMODE		= 472,
	kERR_INVITEONLYCHAN		= 473,
	kERR_BANNEDFROMCHAN		= 474,
	kERR_BADCHANNELKEY		= 475,
	kERR_BADCHANMASK		= 476,
	kERR_NOCHANMODES		= 477,
	kERR_CHANOPRIVSNEEDED	= 482,
	kERR_RESTRICTED			= 484,
	kERR_NOOPERHOST			= 491,
	kERR_UMODEUNKNOWNFLAG	= 501,
	kERR_USERSDONTMATCH		= 502
};

class Reply {
 private:
	 static const std::string	delimiter_;

 public:
	 Reply();
	 ~Reply();
	 const std::string&	getDelimiter() const;
	 std::string		createMessage(int num, const User& user, const Info& info, const ParsedMessage& parsedMsg);
	 static std::string	rplFromName(const std::string& from);
	 static std::string	rplCmdToName(int num, const std::string& toName);

	 static std::string	rplYourHost(const std::string& toName, const std::string& serverName, const std::string& version);
	 static std::string	rplCreated(const std::string& toName, const time_t& createdDate);
	 static std::string	rplMyInfo(const std::string& toName, const Config& config);
	 static std::string	rplWelcome(const Info& info, const User& user);

	 static std::string	rplUModeIs(int num, const std::string& toName, const User& user);
	 // TODO(hnoguchi): const Channel& channelに変更し、全てのmode情報を渡す。
	 static std::string	rplChannelModeIs(int num, const std::string& toName, const std::string& channel, const std::string& mode, const std::string& param);
	 static std::string	rplNoTopic(int num, const std::string& toName, const std::string& channel);
	 static std::string	rplTopic(int num, const std::string& toName, const std::string& channel, const std::string& topic);
	 static std::string	rplInviting(int num, const std::string& toName, const std::string& channel, const std::string& nickName);
	 static std::string	rplYourOper(int num, const std::string& toName, const std::string& nickName);
	 static std::string	errNoSuchNick(int num, const std::string& toName, const std::string& nickName);
	 static std::string	errNoSuchServer(int num, const std::string& toName, const std::string& serverName);
	 static std::string	errNoSuchChannel(int num, const std::string& toName, const std::string& channelName);
	 static std::string	errCanNotSendToChan(int num, const std::string& toName, const std::string& channelName);
	 static std::string	errNoOrigin(int num, const std::string& toName);
	 static std::string	errNoRecipient(int num, const std::string& toName, const std::string& command);
	 static std::string	errNoTextToSend(int num, const std::string& toName);
	 static std::string	errUnknownCommand(int num, const std::string& toName, const std::string& command);
	 static std::string	errNoNickNameGiven(int num, const std::string& toName);
	 static std::string	errOneUsNickName(int num, const std::string& toName, const std::string& nickName);
	 static std::string	errNickNameInUse(int num, const std::string& toName, const std::string& nickName);
	 static std::string	errUserNotInChannel(int num, const std::string& toName, const std::string& nickName, const std::string& channel);
	 static std::string	errNotOnChannel(int num, const std::string& toName, const std::string& channel);
	 static std::string	errUserOnChannel(int num, const std::string& toName, const std::string& nickName, const std::string& channel);
	 static std::string	errNotRegistered(int num, const std::string& toName);
	 static std::string	errNeedMoreParams(int num, const std::string& toName, const std::string& command);
	 static std::string	errAlreadyRegistered(int num, const std::string& toName);
	 static std::string	errPasswordMisMatch(int num, const std::string& toName);
	 static std::string	errKeySet(int num, const std::string& toName, const std::string& nickName, const std::string& channel);
	 static std::string	errChannelIsFull(int num, const std::string& toName, const std::string& channel);
	 static std::string	errUnknownMode(int num, const std::string& toName, const std::string& mode, const std::string& channel);
	 static std::string	errInviteOnlyChan(int num, const std::string& toName, const std::string& channel);
	 static std::string	errBadChannelKey(int num, const std::string& toName, const std::string& channel);
	 static std::string	errNoChanModes(int num, const std::string& toName, const std::string& nickName, const std::string& channel);
	 static std::string	errChanOprivsNeeded(int num, const std::string& toName, const std::string& nickName, const std::string& channel);
	 static std::string	errRestricted(int num, const std::string& toName);
	 static std::string	errUModeUnknownFlag(int num, const std::string& toName);
	 static std::string	errUsersDontMatch(int num, const std::string& toName);
};

#endif  // REPLY_HPP
