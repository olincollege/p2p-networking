#include "./network.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>

uint64_t as_epoll_data(int32_t file_descriptor, int32_t type) {
  epoll_custom_data event_d = {file_descriptor, type};
  uint64_t result;                              // NOLINT
  memcpy(&result, &event_d, sizeof(uint64_t));  // NOLINT
  return result;
}
epoll_custom_data as_custom_data(uint64_t epoll_data_result) {
  epoll_custom_data result = {0, 0};
  memcpy(&result, &epoll_data_result, sizeof(uint64_t));  // NOLINT
  return result;
}

/* Makes a fcntl system call to mark a socket a non-blocking
 * We want to be able to handle multiple IO operations at the same time with
 * epoll https://man7.org/linux/man-pages/man2/fcntl.2.html
 */
void non_blocking_socket(int socket) {
  int flags = fcntl(socket, F_GETFL);
  if (flags < 0) {
    puts("failed to retrieve socket flags");
    exit(1);  // NOLINT
  }
  int set_flag =
      fcntl(socket, F_SETFL, flags | O_NONBLOCK);  // NOLINT intented usage
  if (set_flag < 0) {
    puts("failed to set socket flag");
    exit(1);  // NOLINT
  }
}

/* See issue https://github.com/olincollege/p2p-networking/issues/25. We want to
 * have a large buffer size. */
void large_buffer_socket(int socket) {
  // SO_SNDBUF, SO_SNDBUFFORCE, SO_RCVBUF, SO_RCVBUFFORCE
  int SOCKET_MAX = 1024 * 1024 * 100;  // NOLINT 100 MiB
  int set_flag = setsockopt(socket, SOL_SOCKET, SO_SNDBUFFORCE, &SOCKET_MAX,
                            sizeof(SOCKET_MAX));
  if (set_flag < 0) {
    puts("failed to set socket buffer, make sure you are in sudo mode!");
    exit(1);  // NOLINT
  }
  set_flag = setsockopt(socket, SOL_SOCKET, SO_RCVBUFFORCE, &SOCKET_MAX,
                        sizeof(SOCKET_MAX));
  if (set_flag < 0) {
    puts("failed to set socket buffer, make sure you are in sudo mode!");
    exit(1);  // NOLINT
  }
}

/* Creates a TCP socket and binds it to port
 *
 * If port is 0, binds socket to a random port.
 */
our_server create_socket(uint16_t port) {
  struct sockaddr_in6 server_adress;
  memset(&server_adress, '\0', sizeof(server_adress));   // NOLINT
  server_adress.sin6_family = AF_INET6;                  // use ipv6 resolution
  server_adress.sin6_port = htons(port);                 // port to listen on
  inet_pton(AF_INET6, "::1", &server_adress.sin6_addr);  // listen on localhost

  // try to allocate a TCP socket from OS
  int server_socket = socket(AF_INET6, SOCK_STREAM, 0);
  if (server_socket < 0) {
    puts("failed to allocate socket");
    exit(1);  // NOLINT
  }

  // try to bind socket to port
  int bind_res = bind(server_socket, (const struct sockaddr*)&server_adress,
                      sizeof(server_adress));
  if (bind_res < 0) {
    puts("failed to bind to socket");
    exit(1);  // NOLINT
  }

  // specify maximum backlog of un-accepted connections
  // this dosn't actually do anything, just marks an attribute
  listen(server_socket, MAX_LISTEN_BACKLOG);

  our_server server;
  socklen_t sock_len = sizeof(server_adress);
  getsockname(server_socket, (struct sockaddr*)&server_adress, &sock_len);
  server.file_descriptor = server_socket;
  server.port = ntohs(server_adress.sin6_port);

  return server;
}

/* Create an epoll container for the TCP listening socket.
 * We can use it to drive an I/O loop with many different types of file
 * descriptors.
 *
 * If port is 0, binds socket to a random port.
 */
our_server create_epoll_socket(uint16_t port) {
  // create a socket for our server
  our_server server = create_socket(port);
  int server_socket = server.file_descriptor;
  non_blocking_socket(server_socket);

  // create an epoll container for multiplexing I/O
  // https://man7.org/linux/man-pages/man2/epoll_create.2.html
  int epoll_descriptor = epoll_create1(EPOLL_CLOEXEC);
  if (epoll_descriptor < 0) {
    puts("failed to create epoll descriptor");
    exit(1);  // NOLINT
  }

  // wrap server socket into an epoll event
  struct epoll_event server_epoll;
  server_epoll.data.u64 = as_epoll_data(server_socket, EPOLL_LISTEN_FD);

  // https://man7.org/linux/man-pages/man7/epoll.7.html
  // mark as level-triggering and recieve I/O input events from socket
  server_epoll.events = EPOLLIN;
  if (epoll_ctl(epoll_descriptor, EPOLL_CTL_ADD, server_socket, &server_epoll) <
      0) {
    puts("failed to bind socket to epoll descriptor");
    exit(1);  // NOLINT
  }

  server.file_descriptor = epoll_descriptor;
  return server;
}

size_t full_message_availiable(int socket) {
  uint32_t message[MAX_SIZE_MESSAGE_INT];
  ssize_t message_len_recv = 0;  // Message length received.
  size_t message_len = 0;        // The intended message length.

  // https://pubs.opengroup.org/onlinepubs/007904975/functions/recv.html
  // Peek the message at the socket.
  message_len_recv =
      recv(socket, message, (size_t)MAX_SIZE_MESSAGE_INT * 4, MSG_PEEK);

  if (message_len_recv >= 4) {
    message_len = message[0];
    if ((size_t)message_len_recv >= message_len + 4) {
      return message_len + 4;
    }
  }

  return 0;
}
