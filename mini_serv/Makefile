NAME := mini_serv

$(NAME): mini_serv.c
	gcc -Wall -Wextra -Werror mini_serv.c -o $(NAME)

all: $(NAME)

PHONY: clean
clean:
	rm -f $(NAME)

PHONY: fclean
fclean: clean

PHONY: re
re: fclean all
