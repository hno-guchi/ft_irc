###########
# General #
###########

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic-errors -MMD -MP
DEBUGFLAGS = -g -fsanitize=undefined -fsanitize=integer -fsanitize=address
RM = rm -r -f

#################
# General Rules #
#################

.PHONY: all
all: reply

.PHONY: reply
reply:
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -DDEBUG -o ./reply ./Reply.cpp ../error/error.cpp
	./reply
	$(CXX) $(CXXFLAGS) -DDEBUG -DLEAKS -o ./reply ./Reply.cpp ../error/error.cpp
	./reply
	$(RM) ./reply ./*.d ./*.dSYM
