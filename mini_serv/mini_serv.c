#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

# define RCV_BUF_SIZE 2048
# define TEMPLATE_MSG_SIZE 50
# define ARGS_ERR_MSG "Wrong number of arguments\n"
# define FATAL_ERR_MSG "Fatal error\n"
# define ACCEPTED_MSG "server: client %d just arrived\n"
# define LEFT_MSG "server: client %d just left\n"
# define MSG_SUFIX "client %d: "
# define SAFE_FREE(x) (free(x),(x)=NULL)

typedef struct s_socket t_socket;
typedef struct s_client t_client;
typedef struct s_server t_server;

struct s_socket {
	int					fd;
	struct sockaddr_in	addr;
	unsigned int		addr_len;
};

struct s_client {
	t_socket	socket;
	int			idx;
	char		*msg;
	t_client	*next;
};

struct s_server {
	unsigned short	port;
	t_socket		socket;
	t_client*		clients;
	int				clients_count;
	int				max_fd;
};

// #ifdef DEBUG
void	debug_print_clients(const char* title, t_server* s) {
	printf("[%s]\n", title);
	printf("port         [%d]\n", s->port);
	printf("clients_count[%d]\n", s->clients_count);
	printf("max_fd       [%d]\n", s->max_fd);
	printf("----------------------\n");
	printf("idx[x] : socket.fd[x] | msg\n");
	for (t_client* n = s->clients; n != NULL; n = n->next) {
		printf("   [%d] :          [%d] | [%s]\n", n->idx, n->socket.fd, n->msg);
	}
	printf("----------------------\n\n");
}
// #endif  // DEBUG

static void	error_exit(char *msg) {
	write(STDERR_FILENO, msg, strlen(msg));
	exit (EXIT_FAILURE);
}

static void* ft_xcalloc(size_t count, size_t size) {
	void*	ptr = calloc(count, size);
	if (ptr == NULL) {
		error_exit(FATAL_ERR_MSG);
	}
	return (ptr);
}

static int ft_xaccept(int fd, t_socket *sock) {
	int sockfd = accept(fd, (struct sockaddr*)&sock->addr, &sock->addr_len);
	if (sockfd < 0) {
		error_exit(FATAL_ERR_MSG);
	}
	return (sockfd);
}

static int	ft_xatoi(const char* nptr) {
	int res = atoi(nptr);
	if (res < 1) {
		error_exit(FATAL_ERR_MSG);
	}
	return (res);
}

static int	ft_xsocket(void) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error_exit(FATAL_ERR_MSG);
	}
	return (sockfd);
}

static void	ft_xbind(t_socket* sock) {
	if (bind(sock->fd, (struct sockaddr*)&sock->addr, sock->addr_len) < 0) {
		error_exit(FATAL_ERR_MSG);
	}
}

static void	ft_xlisten(t_socket* sock) {
	if (listen(sock->fd, 3) < 0) {
		error_exit(FATAL_ERR_MSG);
	}
}

static int	ft_xselect(int max_fd, fd_set* r_fds, fd_set* w_fds, fd_set* e_fds, struct timeval* timeout) {
	int	ret = select(max_fd + 1, r_fds, w_fds, e_fds, timeout);
	if (ret < 0) {
		error_exit(FATAL_ERR_MSG);
	}
	return (ret);
}

static ssize_t	ft_xrecv(int fd, char* buf, size_t size) {
	ssize_t	ret = recv(fd, buf, size, MSG_DONTWAIT);
	if (ret < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
		error_exit(FATAL_ERR_MSG);
	}
	return (ret);
}

static void	ft_xsend(int fd, const char* msg) {
	ssize_t	ret = send(fd, msg, strlen(msg), MSG_DONTWAIT);
	if (ret < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
		error_exit(FATAL_ERR_MSG);
	}
}

static t_client*	new_client(int sockfd) {
	if (sockfd < 0) {
		error_exit(FATAL_ERR_MSG);
	}
	t_client* new = (t_client*)ft_xcalloc(1, sizeof(t_client));
	new->idx = -1;
	new->socket.fd = ft_xaccept(sockfd, &(new->socket));
	new->socket.addr_len = sizeof(new->socket.addr);
	return (new);
}

static t_client* lst_last(t_client *lst) {
	if (lst == NULL) {
		return (NULL);
	}
	while (lst->next != NULL) {
		lst = lst->next;
	}
	return (lst);
}

static void	lst_add_back(t_client **lst, t_client *new) {
	if (lst == NULL || new == NULL) {
		return ;
	}
	if (*lst == NULL) {
		*lst = new;
	} else {
		lst_last(*lst)->next = new;
	}
}

static void	delone_client(t_client* c) {
	if (c == NULL) {
		return ;
	}
	close(c->socket.fd);
	c->socket.fd = -1;
	c->socket.addr_len = 0;
	c->idx = -1;
	SAFE_FREE(c->msg);
	SAFE_FREE(c);
}

static void	lst_delone(t_client** lst, t_client* target) {
	if (lst == NULL || *lst == NULL || target == NULL) {
		return ;
	}
	t_client*	current = *lst;
	t_client*	prev = NULL;
	while (current) {
		if (current == target) {
			if (prev) {
				prev->next = current->next;
			} else {
				*lst = current->next;
			}
			return (delone_client(current));
		}
		prev = current;
		current = current->next;
	}
}

static void	set_max_fd(t_server* serv) {
	if (serv == NULL) {
		error_exit(FATAL_ERR_MSG);
	}
	serv->max_fd = serv->socket.fd;
	for (t_client* c = serv->clients; c != NULL; c = c->next) {
		if (c->socket.fd > serv->max_fd) {
			serv->max_fd = c->socket.fd;
		}
	}
}

static void	init_serv(t_server* serv, int port) {
	if (serv == NULL || port < 0) {
		error_exit(FATAL_ERR_MSG);
	}
	serv->port = port;
	serv->socket.fd = ft_xsocket();
	serv->socket.addr_len = sizeof(serv->socket.addr);
	serv->socket.addr.sin_family = AF_INET;
	serv->socket.addr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
	serv->socket.addr.sin_port = htons(serv->port);
	ft_xbind(&(serv->socket));
	ft_xlisten(&(serv->socket));
	// printf("Listening on port [%d]\n", server.port);
}

static void	set_fd_sets(t_server* serv, fd_set* r_fds, fd_set* w_fds) {
	if (serv == NULL || r_fds == NULL || w_fds == NULL) {
		error_exit(FATAL_ERR_MSG);
	}
	FD_ZERO(r_fds);
	FD_ZERO(w_fds);
	FD_SET(serv->socket.fd, r_fds);
	FD_SET(serv->socket.fd, w_fds);
	for (t_client* c = serv->clients; c != NULL; c = c->next) {
		FD_SET(c->socket.fd, r_fds);
		FD_SET(c->socket.fd, w_fds);
	}
}

static void	send_all_clients(t_client* lst, t_client* from, const char* msg, fd_set* w_fds) {
	if (lst == NULL || msg == NULL || w_fds == NULL) {
		error_exit(FATAL_ERR_MSG);
	}
	for (t_client* to = lst; to != NULL; to = to->next) {
		if (from != to && FD_ISSET(to->socket.fd, w_fds)) {
			ft_xsend(to->socket.fd, msg);
		}
	}
}

static void	accept_client(t_server* serv, fd_set* w_fds) {
	if (serv == NULL || w_fds == NULL) {
		error_exit(FATAL_ERR_MSG);
	}
	debug_print_clients("BEFORE: accept_client()", serv);
	t_client*	new = new_client(serv->socket.fd);
	new->idx = serv->clients_count;
	lst_add_back(&serv->clients, new);
	char msg[TEMPLATE_MSG_SIZE] = {0};
	sprintf(msg, ACCEPTED_MSG, new->idx);
	send_all_clients(new, serv->clients, msg, w_fds);
	serv->clients_count += 1;
	debug_print_clients("AFTER : accept_client()", serv);
}

static t_client*	left_client(t_server* serv, t_client* c, fd_set* w_fds) {
	if (serv == NULL || c == NULL || w_fds == NULL) {
		error_exit(FATAL_ERR_MSG);
	}
	debug_print_clients("BEFORE: left_client()", serv);
	t_client*	next = c->next;
	char	msg[TEMPLATE_MSG_SIZE] = {0};
	sprintf(msg, LEFT_MSG, c->idx);
	lst_delone(&serv->clients, c);
	send_all_clients(NULL, serv->clients, msg, w_fds);
	serv->clients_count -= 1;
	debug_print_clients("AFTER : left_client()", serv);
	return (next);
}

static char* ft_strdup(char* s1) {
	if (s1 == NULL) {
		return (NULL);
	}
	char*	ret = (char*)ft_xcalloc(strlen(s1) + 1, sizeof(char));
	return(strcpy(ret, s1));
}

static char*	ft_strjoin(char** s1, char** s2) {
	if (s1 == NULL || s2 == NULL) {
		return (NULL);
	}
	char*	ret = (char*)ft_xcalloc(strlen(*s1) + strlen(*s2) + 1, sizeof(char));
	ret = strcpy(ret, *s1);
	ret = strcat(ret, *s2);
	SAFE_FREE(*s1);
	SAFE_FREE(*s2);
	return (ret);
}

static t_client* handle_message(t_server* serv, t_client* c, char* buf, fd_set* w_fds) {
	if (serv == NULL || c == NULL || buf == NULL || w_fds == NULL) {
		return (NULL);
	}
	char*	msg = ft_strdup(buf);
	if (c->msg != NULL) {
		msg = ft_strjoin(&c->msg, &msg);
	}
	while (msg != NULL) {
		char*	nlp = strstr(msg, "\n");
		if (nlp == NULL) {
			c->msg = ft_strdup(msg);
			SAFE_FREE(msg);
		} else {
			if (*(nlp + 1) != '\0') {
				c->msg = ft_strdup(nlp + 1);
				*(nlp + 1) = '\0';
			}
			char*	sufix = (char*)ft_xcalloc(TEMPLATE_MSG_SIZE, sizeof(char));
			sprintf(sufix, MSG_SUFIX, c->idx);
			char*	send_msg = ft_strjoin(&sufix, &msg);
			send_all_clients(c, serv->clients, send_msg, w_fds);
			SAFE_FREE(send_msg);
			msg = ft_strdup(c->msg);
			SAFE_FREE(c->msg);
		}
	}
	return (c->next);
}

static t_client*	handle_client(t_server* serv, t_client* c, fd_set* r_fds, fd_set* w_fds) {
	if (serv == NULL || c == NULL || r_fds == NULL || w_fds == NULL) {
		return (NULL);
	}
	if (FD_ISSET(c->socket.fd, r_fds)) {
		char	buf[RCV_BUF_SIZE] = {0};
		ssize_t	ret = ft_xrecv(c->socket.fd, buf, RCV_BUF_SIZE - 1);
		if (ret == 0) {
			return (left_client(serv, c, w_fds));
		} else if (ret > 0) {
			return (handle_message(serv, c, buf, w_fds));
		}
	}
	return (c->next);
}

static void	loop_serv(t_server* serv) {
	if (serv == NULL) {
		error_exit(FATAL_ERR_MSG);
	}
	fd_set	read_fds = {0};
	fd_set	write_fds = {0};
	while (1) {
		set_max_fd(serv);
		set_fd_sets(serv, &read_fds, &write_fds);
		int	ret = ft_xselect(serv->max_fd + 1, &read_fds, &write_fds, NULL, NULL);
		if (ret == 0) {
			continue;
		}
		if (FD_ISSET(serv->socket.fd, &read_fds)) {
			accept_client(serv, &write_fds);
		}
		t_client*	next = NULL;
		for (t_client* c = serv->clients; c != NULL; c = next) {
			next = handle_client(serv, c, &read_fds, &write_fds);
		}
	}
}

int	main(int argc, char **argv) {
	if (argc != 2) {
		error_exit(ARGS_ERR_MSG);
	}
	t_server	serv = {0};

	init_serv(&serv, ft_xatoi(argv[1]));
	loop_serv(&serv);
	// destruct_serv(&serv);
	return (EXIT_SUCCESS);
}
