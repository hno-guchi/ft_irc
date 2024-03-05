###########
# General #
###########

NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic-errors -MMD -MP
DEBUGFLAGS = -g -fsanitize=undefined -fsanitize=integer -fsanitize=address -DDEBUG
RM = rm -r -f
SRCS = Server.cpp \
	   Parser.cpp \
	   Execute.cpp \
	   Reply.cpp \
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
$(OBJS_DIR)/%.o: ./parser/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(OBJS_DIR)/%.o: ./execute/%.cpp
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

.PHONY: parser
parser:
	make -C ./parser

.PHONY: reply
reply:
	make -C ./reply
