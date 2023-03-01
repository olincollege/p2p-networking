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

/* Initialize new state instance */
client_state new_state(void);

/* Creates a demo state with random have/want pieces */
client_state demo_state();

/* Deallocate state */
void dealloc_state(client_state *state);

/* Add an owned piece to client state. */
void add_piece_have(client_state *state, void *piece, size_t piece_size);

/* Remove an owned piece from client state. */
void remove_piece_have(client_state *state, void *piece, size_t piece_size);

/* Add a wanted piece to client state. */
void add_piece_want(client_state *state, unsigned char *hash);

/* Remove a wanted piece from client state. */
void remove_piece_want(client_state *state, unsigned char *hash);

/* Add a file descriptor. */
void add_file_descriptor(client_state *state, int file_descriptor);

/* Remove a file descriptor. */
void remove_file_descriptor(client_state *state, int file_descriptor);

/* Add a port. */
void add_port(client_state *state, uint16_t port);

/* Remove a port. */
void remove_port(client_state *state, uint16_t port);
