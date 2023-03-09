#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>

// Constants for our networking configuration
enum {
  MAX_EPOLL_EVENTS = 50,
  MAX_LISTEN_BACKLOG = 50,
  SERVER_LISTEN_PORT = 8100,
  EPOLL_LISTEN_FD = 1, // mark epoll events for our socket with this code
  EPOLL_PEER_FD = 2,   // mark epoll events for peer connections with this code
  EPOLL_TIMEOUT =
      1000, // mark epoll to block this many ms until event is recieved
  MAX_SIZE_MESSAGE_INT = ((1024 * 1024 * 2) / 32) // 2 Mib + 2 ints for overhead
  // this is a little more than needed because the type is only one byte
};

/* Information to describe oursevles to other peers */
typedef struct our_server {
  uint16_t port;
  int file_descriptor;
} our_server;

/* This is kind of a hack but basically the data from returned from an epoll
 * event is a union of this type
 * https://man7.org/linux/man-pages/man2/epoll_ctl.2.html. Ideally, we want to
 * know both the file descriptor and what the descriptor is. The union is 64
 * bits so we can pack a 32 bit file_descriptor and a 32 bit custom user
 * descriptor
 */
typedef struct epoll_custom_data {
  int32_t fd;
  int32_t type;
} epoll_custom_data;

uint64_t as_epoll_data(int32_t file_descriptor, int32_t type);
epoll_custom_data as_custom_data(uint64_t epoll_data_result);

/**
 * Make a socket non-blocking.
 *
 * Makes a fcntl system call to mark a socket a non-blocking
 * We want to be able to handle multiple IO operations at the same time with
 * epoll https://man7.org/linux/man-pages/man2/fcntl.2.html
 *
 * @param socket A socket to make non-blocking.
 */
void non_blocking_socket(int socket);

/**
 * Increase TCP buffer size to 1 MiB.
 *
 * See issue https://github.com/olincollege/p2p-networking/issues/25. We want to
 * have a large buffer size.
 *
 * @param A socket to increase the buffer size of.
 */
void large_buffer_socket(int socket);

/**
 * Create a TCP socket and bind it to port.
 *
 * If specified port is 0, binds to random port.
 *
 * @param port Port number to bind socket to in host byte order.
 * @return A struct with the port number and file descriptor packaged together.
 */
our_server create_socket(uint16_t port);

/**
 * Create an epoll container for the TCP listening socket.
 *
 * If specified port is 0, binds to random port.
 *
 * We can use this to drive an I/O loop with many different types of file
 * descriptors.
 *
 * @param port Port number to bind socket to in host byte order.
 * @return A struct with the port number and epoll FD packed together.
 */
our_server create_epoll_socket(uint16_t port);

/**
 * "Peek" a socket to see if a full message is available.
 *
 * Implemented to make sure that incomplete messages are not cleared out before
 * full message is sent.
 *
 * @param socket A network socket.
 * @return Size of the full message if available, 0 otherwise.
 */
size_t full_message_availiable(int socket);
