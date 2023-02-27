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
  MAX_SIZE_MESSAGE_INT = ((1024 * 1024) / 32) + 2 // 1 Mib + 2 ints for overhead
  //this is a little more than needed because the type is only one byte
};

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

/* Makes a fcntl system call to mark a socket a non-blocking
 * We want to be able to handle multiple IO operations at the same time with
 * epoll https://man7.org/linux/man-pages/man2/fcntl.2.html
 */
void non_blocking_socket(int socket);

/* Creates a TCP socket and binds it to port */
int create_socket(uint16_t port);

/* Create an epoll container for the TCP listening socket.
 * We can use it to drive an I/O loop with many different types of file
 * descriptors.
 */
int create_epoll_socket(uint16_t port);

/* "Peek" a socket to see if a full message is availabe
 * Implemented to make sure that incomplete messages are not cleared out before
 * full message is sent.
 */
int full_message_availiable(int socket);
