CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic-errors -MMD -MP
DEBUGFLAGS = -g -fsanitize=undefined -fsanitize=integer -fsanitize=address -DDEBUG
RM = rm -r -f

.PHONY: parser
parser:
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -o ./parser/parser ./parser/Parser.cpp ./error/error.cpp
	./parser/parser
	$(CXX) $(CXXFLAGS) -DLEAKS -o ./parser/parser ./parser/Parser.cpp ./error/error.cpp
	./parser/parser
	$(RM) ./parser/parser ./parser/*.d ./parser/*.dSYM

.PHONY: reply
reply:
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -o ./reply/reply ./reply/Reply.cpp ./error/error.cpp
	./reply/reply
	$(CXX) $(CXXFLAGS) -DLEAKS -o ./reply/reply ./reply/Reply.cpp ./error/error.cpp
	./reply/reply
	$(RM) ./reply/reply ./reply/*.d ./reply/*.dSYM
