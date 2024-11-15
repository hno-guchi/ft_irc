#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>

# define RCVBUFSIZE 32
# define ARGS_ERR_MSG "Wrong number of arguments\n"
# define FATAL_ERR_MSG "Fatal error\n"

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
	printf("idx[x] : socket.fd[x]\n");
	for (t_list* client = clients; client != NULL; client = client->next) {
		if (client == NULL) {
			break;
		}
		printf("   [%d] :          [%d]\n", client->data->idx, client->data->socket.fd);
	}
	printf("---------------------\n\n");
}

int	main(int argc, char **argv) {
	if (argc != 2) {
		error_exit(ARGS_ERR_MSG);
	}
	t_server	server = {0};
	// _____ INIT SERVER _____
	server.port = ft_xatoi(argv[1]);
	server.socket.fd = ft_xsocket();
	ft_xfcntl(server.socket.fd);
	server.socket.addr_len = sizeof(server.socket.addr);
	server.socket.addr.sin_family = AF_INET;
	server.socket.addr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
	// server.socket.addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// server.socket.addr.sin_addr.s_addr = inet_addr("224.10.10.2");
	server.socket.addr.sin_port = htons(server.port);
	ft_xbind(&(server.socket));
	ft_xlisten(&(server.socket));
	printf("Listening on port [%d]\n", server.port);
	// -----------------------

	struct timeval	timeout = {0};
	int				max_fd = server.socket.fd;
	fd_set			read_fds = {0};
	while (1) {
		FD_ZERO(&read_fds);
		FD_SET(STDIN_FILENO, &read_fds);
		FD_SET(server.socket.fd, &read_fds);
		for (t_list* client = server.clients; client != NULL; client = client->next) {
			if (client == NULL) {
				break;
			}
			FD_SET(client->data->socket.fd, &read_fds);
		}
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		// TODO: error handling
		if ((select(max_fd + 1, &read_fds, NULL, NULL, &timeout)) == 0) {
			// printf("timeout 3 seconds...\n");
			continue;
		}
		if (FD_ISSET(server.socket.fd, &read_fds)) {
			// accept_client(&server);
			t_list*	accepted_client = ft_lstnew(new_client(server.socket.fd));
			accepted_client->data->idx = server.clients_count;
			// send all users
			char msg[41] = {0};
			sprintf(msg, "server: client %d just arrived\n", accepted_client->data->idx);
			for (t_list* target = server.clients; target != NULL; target = target->next) {
				ft_xsend(target->data->socket.fd, msg);
			}
			if (accepted_client->data->socket.fd > max_fd) {
				max_fd = accepted_client->data->socket.fd;
			}
			ft_lstadd_back(&server.clients, accepted_client);
			server.clients_count += 1;
			debug_print_clients(server.clients);
		}
		t_list* client = server.clients;
		while (client != NULL) {
			if (FD_ISSET(client->data->socket.fd, &read_fds)) {
				char	buf[RCVBUFSIZE] = {0};
				ssize_t	recv_size = ft_xrecv(client->data->socket.fd, buf, RCVBUFSIZE - 1);
				if (recv_size == 0) {
					// remove client
					t_list*	next_client = client->next;
					char	msg[36] = {0};
					sprintf(msg, "server: client %d just left\n", client->data->idx);
					server.clients_count -= 1;
					if (client->data->socket.fd == max_fd) {
						max_fd = server.socket.fd;
						for (t_list* target = server.clients; target != NULL; target = target->next) {
							if (target->data->socket.fd > max_fd) {
								max_fd = target->data->socket.fd;
							}
						}
					}
					remove_node(&server.clients, client);
					for (t_list* target = server.clients; target != NULL; target = target->next) {
						ft_xsend(target->data->socket.fd, msg);
					}
					debug_print_clients(server.clients);
					client = next_client;
					continue;
				} else {
					// client %d: <改行までの文字列>の形式で送信
					// char* msg = NULL;
					// if (client->data->msg != NULL) {
					// 	char* nlp = strstr(client->data->msg, "\n");
					// 	if (nlp == NULL) {
					// 		msg = (char*)ft_xcalloc(strlen(client->data->msg), sizeof(char));
					// 	} else {
					// 		msg = (char*)ft_xcalloc(nlp - client->data->msg, sizeof(char));
					// 	}
					// }
					// char* nlp = strstr(buf, "\n");
					// if (nlp == NULL) {
					// 	client->data->msg = (char*)ft_xcalloc(strlen(buf) + 1, sizeof(char));
					// 	client->data->msg = strcpy(client->data->msg, buf);
					// } else if (strlen(nlp) > 1) {
					// 	client->data->msg = (char*)ft_xcalloc((strlen(buf) - strlen(nlp)) + 1, sizeof(char));
					// 	client->data->msg = strcpy(client->data->msg, nlp + 1);
					// }
					// char	sufix[20] = {0};
					// sprintf(sufix, "client %d: ", client->data->idx);
					// for (t_list* target = server.clients; target != NULL; target = target->next) {
					// 	if (target->data->socket.fd != client->data->socket.fd) {
					// 		ft_xsend(target->data->socket.fd, sufix);
					// 		ft_xsend(target->data->socket.fd, buf);
					// 	}
					// }
				}
			}
			client = client->next;
		}
	}
	return (EXIT_SUCCESS);
}
