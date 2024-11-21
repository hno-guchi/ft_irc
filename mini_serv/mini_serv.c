#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

# define RCV_BUF_SIZE 2048
# define SUFIX_SIZE 20
# define ACCEPTED_MSG_SIZE 41
# define LEFT_MSG_SIZE 36
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

static int ft_xaccept(int fd, t_socket *socket) {
	int new_socket = accept(fd, (struct sockaddr*)&socket->addr, &socket->addr_len);
	if (new_socket < 0) {
		error_exit(FATAL_ERR_MSG);
	}
	return (new_socket);
}

static int	ft_xatoi(const char* nptr) {
	int res = atoi(nptr);
	if (res < 1) {
		error_exit(FATAL_ERR_MSG);
	}
	return (res);
}

static int	ft_xsocket() {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		error_exit(FATAL_ERR_MSG);
	}
	return (sock);
}

static void	ft_xbind(t_socket* socket) {
	if (bind(socket->fd, (struct sockaddr*)&socket->addr, socket->addr_len)) {
		error_exit(FATAL_ERR_MSG);
	}
}

static void	ft_xlisten(t_socket* socket) {
	if (listen(socket->fd, 3)) {
		error_exit(FATAL_ERR_MSG);
	}
}

static int	ft_xselect(int max_fd, fd_set* r_fds, fd_set* w_fds, fd_set* e_fds, struct timeval* timeout) {
	int	result = select(max_fd + 1, r_fds, w_fds, e_fds, timeout);
	if (result < 0) {
		error_exit(FATAL_ERR_MSG);
	}
	return (result);
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

static t_client*	new_client(int socket_fd) {
	if (socket_fd < 0) {
		error_exit(FATAL_ERR_MSG);
	}
	t_client* client = (t_client*)ft_xcalloc(1, sizeof(t_client));
	client->socket.addr_len = sizeof(client->socket.addr);
	client->socket.fd = ft_xaccept(socket_fd, &(client->socket));
	client->idx = -1;
	client->next = NULL;
	return (client);
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

static void	client_delone(t_client* client) {
	if (client == NULL) {
		return ;
	}
	close(client->socket.fd);
	client->socket.fd = -1;
	client->socket.addr_len = 0;
	client->idx = -1;
	if (client->msg != NULL) {
		SAFE_FREE(client->msg);
	}
	SAFE_FREE(client);
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
			client_delone(current);
			return;
		}
		prev = current;
		current = current->next;
	}
}

static void	set_max_fd(t_server* server) {
	server->max_fd = server->socket.fd;
	for (t_client* n = server->clients; n != NULL; n = n->next) {
		if (n->socket.fd > server->max_fd) {
			server->max_fd = n->socket.fd;
		}
	}
}

static void	init_server_socket(t_server* server, int port) {
	server->port = port;
	server->socket.fd = ft_xsocket();
	server->socket.addr_len = sizeof(server->socket.addr);
	server->socket.addr.sin_family = AF_INET;
	server->socket.addr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
	server->socket.addr.sin_port = htons(server->port);
	ft_xbind(&(server->socket));
	ft_xlisten(&(server->socket));
	// printf("Listening on port [%d]\n", server.port);
}

static void	set_fd_sets(t_server* server, fd_set* r_fds, fd_set* w_fds) {
	FD_ZERO(r_fds);
	FD_ZERO(w_fds);
	FD_SET(STDIN_FILENO, r_fds);
	FD_SET(server->socket.fd, r_fds);
	FD_SET(server->socket.fd, w_fds);
	for (t_client* n = server->clients; n != NULL; n = n->next) {
		FD_SET(n->socket.fd, r_fds);
		FD_SET(n->socket.fd, w_fds);
	}
}

static void	send_all_clients(t_client* from, t_client* lst, const char* msg, fd_set* w_fds) {
	for (t_client* to = lst; to != NULL; to = to->next) {
		if (from != to && FD_ISSET(to->socket.fd, w_fds)) {
			ft_xsend(to->socket.fd, msg);
		}
	}
}

static void	accept_client(t_server* server, fd_set* w_fds) {
	debug_print_clients("BEFORE: accept_client()", server);
	t_client*	new = new_client(server->socket.fd);
	new->idx = server->clients_count;
	lst_add_back(&server->clients, new);
	char msg[ACCEPTED_MSG_SIZE] = {0};
	sprintf(msg, ACCEPTED_MSG, new->idx);
	send_all_clients(new, server->clients, msg, w_fds);
	server->clients_count += 1;
	debug_print_clients("AFTER : accept_client()", server);
}

static t_client*	left_client(t_server* server, t_client* client, fd_set* w_fds) {
	debug_print_clients("BEFORE: left_client()", server);
	t_client*	next = client->next;
	char	msg[LEFT_MSG_SIZE] = {0};
	sprintf(msg, LEFT_MSG, client->idx);
	lst_delone(&server->clients, client);
	send_all_clients(NULL, server->clients, msg, w_fds);
	server->clients_count -= 1;
	debug_print_clients("AFTER : left_client()", server);
	return (next);
}

static char* ft_strdup(char* s1) {
	if (s1 == NULL) {
		return (NULL);
	}
	char*	ret = (char*)ft_xcalloc(strlen(s1) + 1, sizeof(char));
	ret = strcpy(ret, s1);
	return (ret);
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

static t_client* handle_message(t_server* server, t_client* client, char* buf, fd_set* w_fds) {
	char*	msg = ft_strdup(buf);
	if (client->msg != NULL) {
		msg = ft_strjoin(&client->msg, &msg);
	}
	while (msg != NULL) {
		char*	nlp = strstr(msg, "\n");
		if (nlp == NULL) {
			client->msg = ft_strdup(msg);
			SAFE_FREE(msg);
		} else {
			if (*(nlp + 1) != '\0') {
				client->msg = ft_strdup(nlp + 1);
				*(nlp + 1) = '\0';
			}
			char*	sufix = (char*)ft_xcalloc(SUFIX_SIZE, sizeof(char));
			sprintf(sufix, MSG_SUFIX, client->idx);
			char*	send_msg = ft_strjoin(&sufix, &msg);
			send_all_clients(client, server->clients, send_msg, w_fds);
			SAFE_FREE(send_msg);
			msg = ft_strdup(client->msg);
			SAFE_FREE(client->msg);
		}
	}
	return (client->next);
}

static void	handle_clients(t_server* server, fd_set* r_fds, fd_set* w_fds) {
	t_client*	client = server->clients;
	while (client != NULL) {
		if (FD_ISSET(client->socket.fd, r_fds)) {
			char	buf[RCV_BUF_SIZE] = {0};
			ssize_t	recv_size = ft_xrecv(client->socket.fd, buf, RCV_BUF_SIZE - 1);
			if (recv_size < 0) {
				client = client->next;
			} else if (recv_size == 0) {
				client = left_client(server, client, w_fds);
			} else {
				client = handle_message(server, client, buf, w_fds);
			}
		} else {
			client = client->next;
		}
	}
}

static void	run(t_server* server) {
	fd_set	read_fds = {0};
	fd_set	write_fds = {0};
	while (1) {
		set_max_fd(server);
		set_fd_sets(server, &read_fds, &write_fds);
		int	result = ft_xselect(server->max_fd + 1, &read_fds, &write_fds, NULL, NULL);
		if (result < 0) {
			error_exit(FATAL_ERR_MSG);
		} else if (result == 0) {
			// printf("timeout NULL...\n");
			continue;
		}
		if (FD_ISSET(STDIN_FILENO, &read_fds)) {
			break ;
		}
		if (FD_ISSET(server->socket.fd, &read_fds)) {
			accept_client(server, &write_fds);
		}
		handle_clients(server, &read_fds, &write_fds);
	}
}

int	main(int argc, char **argv) {
	if (argc != 2) {
		error_exit(ARGS_ERR_MSG);
	}
	t_server	server = {0};

	init_server_socket(&server, ft_xatoi(argv[1]));
	run(&server);
	// system("leaks mini_serv");
	return (EXIT_SUCCESS);
}
