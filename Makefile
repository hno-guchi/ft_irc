###########
# General #
###########

NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic-errors -MMD -MP
DEBUGFLAGS = -g -fsanitize=address -fsanitize=undefined -fsanitize=integer
LEAKSFLAG = -DSERVER_LEAKS
RM = rm -r -f
SRCS = Server.cpp \
	   ServerSocket.cpp \
	   Config.cpp \
	   Info.cpp \
	   User.cpp \
	   Channel.cpp \
	   Parser.cpp \
	   Execute.cpp \
	   Reply.cpp \
	   cmdNick.cpp cmdUser.cpp cmdUserMode.cpp cmdOper.cpp \
	   cmdPong.cpp \
	   cmdNotice.cpp cmdPrivmsg.cpp \
	   cmdJoin.cpp \
	   error.cpp
OBJS = $(addprefix $(OBJS_DIR)/, $(SRCS:%.cpp=%.o))
OBJS_DIR = ./objs
DEPENDS = $(addprefix $(OBJS_DIR)/, $(OBJS:%.o=%.d))

#################
# General Rules #
#################

.PHONY: all
all: $(NAME)

$(NAME): $(OBJS_DIR) $(OBJS)
	$(CXX) -o $@ $(OBJS)

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

$(OBJS_DIR)/%.o: ./server/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(OBJS_DIR)/%.o: ./user/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(OBJS_DIR)/%.o: ./channel/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(OBJS_DIR)/%.o: ./parser/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(OBJS_DIR)/%.o: ./execute/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(OBJS_DIR)/%.o: ./execute/channel_operations/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(OBJS_DIR)/%.o: ./execute/connection_registration/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(OBJS_DIR)/%.o: ./execute/miscellaneous/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(OBJS_DIR)/%.o: ./execute/sending/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(OBJS_DIR)/%.o: ./reply/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(OBJS_DIR)/%.o: ./error/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) *.dSYM
	$(RM) $(OBJS_DIR)

.PHONY: fclean
fclean: clean
	$(RM) $(NAME)

.PHONY: re
re: fclean all

.PHONY: debug
debug: fclean
debug: CXXFLAGS = $(DEBUGFLAGS)
debug: $(OBJS_DIR)
debug: $(OBJS)
debug:
	$(CXX) $(DEBUGFLAGS) -o $(NAME) $(OBJS)

.PHONY: leaks
leaks: re
leaks:
	leaks -q --atExit -- ./$(NAME)

.PHONY: parser
parser:
	make -C ./parser

.PHONY: reply
reply:
	make -C ./reply

.PHONY: user
user: $(OBJS_DIR)
	$(CXX) $(CXXFLAGS) -o $(OBJS_DIR)/user ./user/User.cpp
