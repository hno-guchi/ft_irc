CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic-errors -MMD -MP
DEBUGFLAGS = -g -fsanitize=undefined -fsanitize=integer -fsanitize=address -DDEBUG

.PHONY: parser
parser:
	$(CXX) $(CXXFLAGS) -o ./parser/parser ./parser/MessageParse.cpp
	./parser/parser
