#ifndef SENDMSG_HPP
# define SENDMSG_HPP

# include <string>

class SendMsg {
 private:
	 int			*fd_;
	 std::string	prefix_;
	 std::string	msg_;
 public:
	 explicit SendMsg(int *fd = NULL);
	 ~SendMsg();
	 // SETTER
	 void		setPrefix(const std::string& prefix);
	 void		setMsg(const std::string& msg);
	 // GETTER
	 const std::string&	getPrefix() const;
	 const std::string&	getMsg() const;
};
#endif  // SENDMSG_HPP
