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
	t_client	*data;
	t_list		*next;
};

struct s_server {
	unsigned short	port;
	t_socket		socket;
	t_list			*clients;
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

static t_list* ft_lstnew(void* content) {
	t_list* new = (t_list*)ft_xcalloc(1, sizeof(t_list));
	new->data = content;
	new->next = NULL;
	return (new);
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

static void ft_lstadd_back(t_list **lst, t_list *new) {
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

static t_client* new_client(int socket_fd) {
	if (socket_fd < 0) {
		error_exit(FATAL_ERR_MSG);
	}
	t_client* client = (t_client*)ft_xcalloc(1, sizeof(t_client));
	client->socket.addr_len = sizeof(client->socket.addr);
	client->socket.fd = ft_xaccept(socket_fd, &(client->socket));
	return (client);
}

int	main(int argc, char **argv) {
	if (argc != 2) {
		error_exit(ARGS_ERR_MSG);
	}
	t_server	server = {0};
	server.socket.addr_len = sizeof(server.socket.addr);
	server.port = atoi(argv[1]);
	if (server.port < 1) {
		error_exit(FATAL_ERR_MSG);
	}
	server.socket.fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server.socket.fd < 0) {
		error_exit(FATAL_ERR_MSG);
	}
	server.socket.addr.sin_family = AF_INET;
	server.socket.addr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
	// server.socket.addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// server.socket.addr.sin_addr.s_addr = inet_addr("224.10.10.2");
	server.socket.addr.sin_port = htons(server.port);

	if (bind(server.socket.fd, (struct sockaddr*)&server.socket.addr, server.socket.addr_len)) {
		error_exit(FATAL_ERR_MSG);
	}
	if (listen(server.socket.fd, 3)) {
		error_exit(FATAL_ERR_MSG);
	}

	printf("Listening on port [%d]\n", server.port);

	int		max_fd = server.socket.fd;
	fd_set	read_fds = {0};
	// fd_set	write_fds;
	// fd_set	exception_fds;
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
		// TODO: error handling
		if ((select(max_fd + 1, &read_fds, NULL, NULL, NULL)) == 0) {
			continue;
		}
		if (FD_ISSET(server.socket.fd, &read_fds)) {
			// accept_client(&server);
			ft_lstadd_back(&server.clients, ft_lstnew(new_client(server.socket.fd)));
			for (t_list* client = server.clients; client != NULL; client = client->next) {
				if (client->data->socket.fd > max_fd) {
					max_fd = client->data->socket.fd;
				}
			}
			ft_lstlast(server.clients)->data->idx = server.clients_count;
			// send all users 41
			char msg[41] = {0};
			sprintf(msg, "server: client %d just arrived\n", server.clients_count);
			for (t_list* target = server.clients; target != NULL; target = target->next) {
				if (send(target->data->socket.fd, msg, strlen(msg), 0) != (ssize_t)strlen(msg)) {
					write(STDERR_FILENO, "send error\n", 11);
				}
			}
			server.clients_count += 1;
			// continue;
		}
		for (t_list* client = server.clients; client != NULL; client = client->next) {
			if (client == NULL) {
				break;
			}
			if (FD_ISSET(client->data->socket.fd, &read_fds)) {
				// recv and send
				char	buffer[RCVBUFSIZE] = {0};
				// int		received_size = 0;
				// while ((received_size = recv(client->data->socket.fd, buffer, RCVBUFSIZE - 1, 0)) != 0) {
					int received_size = recv(client->data->socket.fd, buffer, RCVBUFSIZE - 1, 0);
					if (received_size < 0) {
						error_exit(FATAL_ERR_MSG);
					}
					buffer[received_size] = '\0';
					// printf("client %d: %s", client->data->idx, buffer);
					for (t_list* target = server.clients; target != NULL; target = target->next) {
						if (send(target->data->socket.fd, buffer, received_size, 0) != received_size) {
							error_exit(FATAL_ERR_MSG);
						}
					}
				// }
			}
		}
		// close(client.socket_fd);
		// printf("server: client %d just left\n", 0);
	}
	return (EXIT_SUCCESS);
}
