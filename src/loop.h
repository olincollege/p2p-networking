#pragma once

#include "./message.h"
#include "./state.h"

/**
 * Run when a new client has connected to the epoll socket.
 *
 * When run, all new clients to the socket are accepted to the listening queue.
 *
 * @param epoll_c File descriptor of epoll container
 * @param file_descriptor FD of new connection
 */
void on_connection(int epoll_c, int file_descriptor);

/**
 * Main I/O loop of program
 *
 * Arguments to this function are the global state of the program.
 *
 * @param epoll_c File descriptor of epoll container
 * @param events An array of epoll events
 * @param state Global state of client
 */
void loop(int epoll_c, struct epoll_event *events, client_state *state);

/**
 * Read a message from a client and update the state.
 *
 * @param file_descriptor A client's socket
 * @param epoll_fd File descriptor of epoll container
 * @param state Global state of client
 */
void read_message(int file_descriptor, int epoll_fd, client_state *state);

/**
 * Connect to a peer.
 *
 * If peer is already connected, does nothing.
 *
 * @param peer Peer to attempt to connect to
 * @param epoll_c File descriptor of epoll container
 * @return File descriptor if successful, 0 if failed.
 */
int connect_to_peer(peer_info peer, int epoll_c);

/**
 * Connect to an array of peers.
 *
 * @param peer_list An array of peers to attempt to connect to
 * @param n Number of elements in peer_list
 * @param state Global state of client
 * @param epoll_c File descriptor of epoll container
 */
void connect_to_list(peer_info *peer_list, size_t n, client_state *state,
                     int epoll_c);

/**
 * Bind a file descriptor to an epoll socket.
 *
 * @param epoll_c File descriptor of epoll container
 * @param file_descriptor A file descriptor to bind to the epoll socket
 * @return Result of the bind operation from epoll_ctl system call
 */
int bind_epoll(int epoll_c, int file_descriptor);
