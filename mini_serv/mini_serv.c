#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>

# define RCVBUFSIZE 2048
# define ARGS_ERR_MSG "Wrong number of arguments\n"
# define FATAL_ERR_MSG "Fatal error\n"
# define ACCEPTED_MSG "server: client %d just arrived\n"
# define LEFT_MSG "server: client %d just left\n"
# define SAFE_FREE(x) (free(x),(x)=NULL)

typedef struct s_socket t_socket;
typedef struct s_client t_client;
typedef struct s_list t_list;
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
};

struct s_list {
	t_client*	data;
	t_list*		next;
};

struct s_server {
	unsigned short	port;
	t_socket		socket;
	t_list*			clients;
	int				clients_count;
	int				max_fd;
};

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

static void ft_xfcntl(int fd) {
	if (fcntl(fd, F_SETFL, O_NONBLOCK)) {
		close(fd);
		error_exit(FATAL_ERR_MSG);
	}
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

static int ft_xselect(int max_fd, fd_set* read, fd_set* write, fd_set* except, struct timeval* timeout) {
	int result = select(max_fd + 1, read, write, except, timeout);
	if (result < 0) {
		error_exit(FATAL_ERR_MSG);
	}
	return (result);
}

static ssize_t	ft_xrecv(int fd, char* buf, size_t size) {
	ssize_t ret = recv(fd, buf, size, MSG_DONTWAIT);
	if (ret < 0) {
		error_exit(FATAL_ERR_MSG);
	}
	return (ret);
}

static void	ft_xsend(int fd, const char* msg) {
	if (send(fd, msg, strlen(msg), MSG_DONTWAIT) < 0) {
		error_exit(FATAL_ERR_MSG);
	}
}

static t_list*	ft_lstnew(void* content) {
	t_list* new = (t_list*)ft_xcalloc(1, sizeof(t_list));
	new->data = content;
	new->next = NULL;
	return (new);
}

static t_client*	new_client(int socket_fd) {
	if (socket_fd < 0) {
		error_exit(FATAL_ERR_MSG);
	}
	t_client* client = (t_client*)ft_xcalloc(1, sizeof(t_client));
	client->socket.addr_len = sizeof(client->socket.addr);
	client->socket.fd = ft_xaccept(socket_fd, &(client->socket));
	ft_xfcntl(client->socket.fd);
	return (client);
}

static t_list* ft_lstlast(t_list *lst) {
	if (lst == NULL) {
		return (NULL);
	}
	while (lst->next != NULL) {
		lst = lst->next;
	}
	return (lst);
}

static void	ft_lstadd_back(t_list **lst, t_list *new) {
	if (lst == NULL || new == NULL) {
		return ;
	}
	if (*lst == NULL) {
		*lst = new;
	} else {
		ft_lstlast(*lst)->next = new;
	}
	return ;
}

static void	ft_lstdelone(t_list* lst, void (*del)(t_client*)) {
	if (lst == NULL || del == NULL) {
		return ;
	}
	if (lst->data != NULL) {
		del(lst->data);
	}
	lst->data = NULL;
	lst->next = NULL;
	free(lst);
	lst = NULL;
}

// static void ft_lstclear(t_list** lst, void (*del)(t_client*)) {
// 	if (lst == NULL || *lst == NULL || del == NULL) {
// 		return ;
// 	}
// 	for (t_list* next = (*lst)->next; *lst != NULL; *lst = next) {
// 		next = (*lst)->next;
// 		ft_lstdelone(*lst, del);
// 	}
// 	lst = NULL;
// }

static void	client_delone(t_client* client) {
	if (client == NULL) {
		return ;
	}
	close(client->socket.fd);
	client->socket.fd = -1;
	client->socket.addr_len = 0;
	client->idx = -1;
	if (client->msg != NULL) {
		free(client->msg);
	}
	free(client);
}

static void	remove_node(t_list** lst, t_list* target) {
	if (lst == NULL || *lst == NULL || target == NULL) {
		return ;
	}
	t_list*	head = *lst;
	if (head == target) {
		head = head->next;
	}
	t_list*	prev = *lst;
	while (*lst != NULL) {
		if (*lst == target) {
			prev->next = (*lst)->next;
			ft_lstdelone(*lst, client_delone);
			break ;
		}
		prev = *lst;
		*lst = (*lst)->next;
	}
	*lst = head;
}

void	debug_print_clients(t_list* clients) {
	printf("idx[x] : socket.fd[x] | msg\n");
	for (t_list* client = clients; client != NULL; client = client->next) {
		if (client == NULL) {
			break;
		}
		printf("   [%d] :          [%d] | [%s]\n", client->data->idx, client->data->socket.fd, client->data->msg);
	}
	printf("---------------------\n\n");
}

static void	set_max_fd(t_server* server) {
	server->max_fd = server->socket.fd;
	for (t_list* client = server->clients; client != NULL; client = client->next) {
		if (client == NULL) {
			break;
		}
		if (client->data->socket.fd > server->max_fd) {
			server->max_fd = client->data->socket.fd;
		}
	}
}

static void	init_server_socket(t_server* server, int port) {
	server->port = port;
	server->socket.fd = ft_xsocket();
	ft_xfcntl(server->socket.fd);
	server->socket.addr_len = sizeof(server->socket.addr);
	server->socket.addr.sin_family = AF_INET;
	server->socket.addr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1 // htonl(INADDR_ANY); // inet_addr("224.10.10.2");
	server->socket.addr.sin_port = htons(server->port);
	ft_xbind(&(server->socket));
	ft_xlisten(&(server->socket));
	// printf("Listening on port [%d]\n", server.port);
}

static void	set_fd_set(t_server* server, fd_set* fds) {
	set_max_fd(server);
	FD_ZERO(fds);
	// FD_SET(STDIN_FILENO, read_fds);
	FD_SET(server->socket.fd, fds);
	for (t_list* client = server->clients; client != NULL; client = client->next) {
		if (client == NULL) {
			break;
		}
		FD_SET(client->data->socket.fd, fds);
	}
}

static void	send_all_clients(t_list* from, t_list* clients, const char* msg) {
	for (t_list* target = clients; target != NULL; target = target->next) {
		if (from == NULL || from->data != target->data) {
			ft_xsend(target->data->socket.fd, msg);
		}
	}
}

static void	accept_client(t_server* server) {
	t_list*	accepted_client = ft_lstnew(new_client(server->socket.fd));
	accepted_client->data->idx = server->clients_count;
	ft_lstadd_back(&server->clients, accepted_client);
	char msg[41] = {0};
	sprintf(msg, ACCEPTED_MSG, accepted_client->data->idx);
	send_all_clients(accepted_client, server->clients, msg);
	server->clients_count += 1;
	debug_print_clients(server->clients);
}

static void	remove_client(t_server* server, t_list* client) {
	char	msg[36] = {0};
	sprintf(msg, LEFT_MSG, client->data->idx);
	server->clients_count -= 1;
	remove_node(&server->clients, client);
	send_all_clients(NULL, server->clients, msg);
	debug_print_clients(server->clients);
}

static void	run(t_server* server) {
	struct timeval	timeout = {0};
	fd_set			read_fds = {0};
	while (1) {
		set_fd_set(server, &read_fds);
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		int	result = ft_xselect(server->max_fd + 1, &read_fds, NULL, NULL, &timeout);
		if (result == 0) {
			// printf("timeout 3 seconds...\n");
			continue;
		}
		if (FD_ISSET(server->socket.fd, &read_fds)) {
			accept_client(server);
		}
		t_list*	client = server->clients;
		while (client != NULL) {
			if (FD_ISSET(client->data->socket.fd, &read_fds)) {
				char	buf[RCVBUFSIZE] = {0};
				ssize_t	recv_size = ft_xrecv(client->data->socket.fd, buf, RCVBUFSIZE - 1);
				if (recv_size == 0) {
					t_list*	next_client = client->next;
					remove_client(server, client);
					client = next_client;
					continue;
				} else {
					char*	msg = NULL;
					if (client->data->msg != NULL) {
						msg = (char*)ft_xcalloc(strlen(client->data->msg) + strlen(buf) + 1, sizeof(char));
						msg = strcpy(msg, client->data->msg);
						msg = strcat(msg, buf);
						SAFE_FREE(client->data->msg);
					} else {
						msg = (char*)ft_xcalloc(strlen(buf) + 1, sizeof(char));
						msg = strcpy(msg, buf);
					}
					while (1) {
						if (msg == NULL) {
							break;
						}
						char*	nlp = strstr(msg, "\n");
						if (nlp == NULL) {
							client->data->msg = (char*)ft_xcalloc(strlen(msg) + 1, sizeof(char));
							client->data->msg = strcpy(client->data->msg, msg);
							SAFE_FREE(msg);
							// debug_print_clients(server->clients);
							break;
						} else {
							if (*(nlp + 1) != '\0') {
								client->data->msg = (char*)ft_xcalloc(strlen(nlp + 1) + 1, sizeof(char));
								client->data->msg = strcpy(client->data->msg, nlp + 1);
								*(nlp + 1) = '\0';
							}
							char	sufix[20] = {0};
							sprintf(sufix, "client %d: ", client->data->idx);
							for (t_list* target = server->clients; target != NULL; target = target->next) {
								if (target->data->socket.fd != client->data->socket.fd) {
									ft_xsend(target->data->socket.fd, sufix);
									ft_xsend(target->data->socket.fd, msg);
								}
							}
							SAFE_FREE(msg);
							if (client->data->msg == NULL) {
								break;
							}
							msg = ft_xcalloc(strlen(client->data->msg) + 1, sizeof(char));
							msg = strcpy(msg, client->data->msg);
							SAFE_FREE(client->data->msg);
							// printf("%s", msg);
						}
					}
				}
			}
			client = client->next;
		}
	}

}

int	main(int argc, char **argv) {
	if (argc != 2) {
		error_exit(ARGS_ERR_MSG);
	}
	t_server	server = {0};

	init_server_socket(&server, ft_xatoi(argv[1]));
	run(&server);
	return (EXIT_SUCCESS);
}
