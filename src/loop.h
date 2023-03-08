#pragma once

#include "./message.h"
#include "./state.h"

/* When epoll notifies us of a connnection of our socket, we want to accept all
 * of them and add them to our listening queue.
 */
void on_connection(int epoll_c, int file_descriptor);

/* Main I/O loop of our program */
void loop(int epoll_c, struct epoll_event* events, client_state* state);

/* Read a message from a client and update the state */
void read_message(int file_descriptor, int epoll_fd, client_state* state);

/* Connect to a peer
 * Returns file descriptor if successful, 0 if failed
 */
int connect_to_peer(peer_info peer, int epoll_c);

// Connect to an array of peer
void connect_to_list(peer_info* peer_list, size_t n, client_state* state,
                     int epoll_c);

// Bind a file descriptor to an epoll socket.
int bind_epoll(int epoll_c, int file_descriptor);
