###########
# General #
###########

NAME = parser
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic-errors -MMD -MP
DEBUGFLAGS = -g -fsanitize=address
# DEBUGFLAGS = -g -fsanitize=undefined, integer, address
LEAKSFLAG = -DSERVER_LEAKS
RM = rm -r -f
SRCS = main.cpp Parser.cpp ParsedMsg.cpp Param.cpp Token.cpp ValidParam.cpp IsChar.cpp
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

$(OBJS_DIR)/%.o: ./%.cpp
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

.PHONY: tokenize
tokenize: CXXFLAGS += -DUTEST_TOKENIZE
tokenize: re

.PHONY: validparam
validparam:
	$(CXX) $(CXXFLAGS) -DUTEST_VALIDPARAM -DLEAKS -o ./validparam ./ValidParam.cpp ./IsChar.cpp
	# ./validparam
	# $(CXX) $(DEBUGFLAGS) -DUTEST_VALID -o ./validparam ./ValidParam.cpp ./IsChar.cpp
	# ./validparam
	# $(RM) ./validparam ./*.d ./*.dSYM

.PHONY: ischar
ischar:
	$(CXX) $(CXXFLAGS) -DUTEST_ISCHAR -DLEAKS -o ./ischar ./IsChar.cpp
	./ischar
	$(CXX) $(DEBUGFLAGS) -DUTEST_ISCHAR -o ./ischar ./IsChar.cpp
	./ischar
	$(RM) ./ischar ./*.d ./*.dSYM

