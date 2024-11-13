NAME := mini_serv

$(NAME): mini_serv.c
	cc -Wall -Wextra -Werror mini_serv.c -o $(NAME)

all: $(NAME)

PHONY: clean
clean:
	$(RM) -r $(NAME)
	$(RM) -r $(NAME).dSYM

PHONY: fclean
fclean: clean

PHONY: re
re: fclean all

PHONY: debug
debug: fclean
	cc -g -fsanitize=address mini_serv.c -o $(NAME)
