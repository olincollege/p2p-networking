#pragma once

#include <stdint.h>

#include "./hash_table.h"

/* Struct representing state of the p2p client
 *
 * Keeps track of pieces we have, pieces we want, file descriptors of other
 * clients and port numbers.
 *
 * Uses hash table as a set.
 */
typedef struct client_state {
  hash_table pieces_have;
  hash_table pieces_want;
  hash_table file_descriptors;
  hash_table ports;
} client_state;

/**
 * Initialize a new state instance.
 *
 * All hash tables in the returned struct are initialized and empty.
 *
 * @return An initialized client_state object.
 */
client_state new_state(void);

/**
 * Create a demo state with random have/want pieces
 *
 * @return A client_state object populated with random pieces.
 */
client_state demo_state();

/**
 * Deallocate client_state object.
 *
 * Frees all hash tables in the client_state object with hash_dealloc.
 *
 * @param
 */
void dealloc_state(client_state *state);

/**
 * Add an owned piece to client state.
 *
 * @param state Location in memory of the client_state object
 * @param piece Location in memory of piece
 * @param piece_size Number of bytes in memory piece occupies
 */
void add_piece_have(client_state *state, void *piece, size_t piece_size);

/**
 * Remove an owned piece from client state.
 *
 * @param state Location in memory of the client_state object
 * @param piece Location in memory of piece
 * @param piece_size Number of bytes in memory piece occupies
 */
void remove_piece_have(client_state *state, void *piece, size_t piece_size);

/**
 * Add a wanted piece to client state.
 *
 * Hash is assumed to be a 256 bit (32 byte) sha256 hash.
 *
 * @param state Location in memory of the client_state object
 * @param hash Location in memory of wanted piece's sha256 hash
 */
void add_piece_want(client_state *state, unsigned char *hash);

/**
 * Remove a wanted piece from client state.
 *
 * @param state Location in memory of the client_state object
 * @param hash Location in memory of wanted piece's sha256 hash
 */
void remove_piece_want(client_state *state, unsigned char *hash);

/**
 * Add a peer file descriptor to client state.
 *
 * @param state Location in memory of the client_state object
 * @param file_descriptor File descriptor of connected peer
 */
void add_file_descriptor(client_state *state, int file_descriptor);

/**
 * Remove a peer file descriptor from client state.
 *
 * @param state Location in memory of the client_state object
 * @param file_descriptor File descriptor of formerly connected peer.
 */
void remove_file_descriptor(client_state *state, int file_descriptor);

/**
 * Add a port to client state.
 *
 * @param state Location in memory of the client_state object
 * @param port A port number in host byte order
 */
void add_port(client_state *state, uint16_t port);

/**
 * Remove a port from client state.
 *
 * @param state Location in memory of the client_state object
 * @param port A port number in host byte order
 */
void remove_port(client_state *state, uint16_t port);

/**
 * Send piece to a peer if it is in state.
 *
 * Function is called when receiving an ask message from a peer. If the client
 * has the requested piece, it sends the piece to the peer.
 *
 * @param state Location in memory of the client_state object
 * @param message A received message from a peer requesting a piece of data.
 * @param peer File descriptor of connected peer asking for the piece.
 */
void send_if_have(client_state *state, ask_message message, int peer);

/**
 * Exchange peer list with connected clients.
 *
 * @param state Location in memory of the client_state object
 */
void peer_exchange(client_state *state);

/**
 * Broadcast wanted pieces to connect clients
 *
 * @param state Location in memory of the client_state object
 */
void broadcast_want(client_state *state);
