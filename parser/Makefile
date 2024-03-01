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
all: parser

.PHONY: parser
parser:
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -DDEBUG -o ./parser ./Parser.cpp ../error/error.cpp
	./parser
	$(CXX) $(CXXFLAGS) -DDEBUG -DLEAKS -o ./parser ./Parser.cpp ../error/error.cpp
	./parser
	$(RM) ./parser ./*.d ./*.dSYM
