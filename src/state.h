#pragma once

#include "hash_table.h"
#include <cstddef>
#include <cstdint>
#include <stdint.h>

/* Struct representing a connected client. */
typedef struct connected_client {
  FILE* descriptor;
  uint16_t port;
} connected_client;

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
  hash_table clients_connected;
} client_state;

/* Add an owned piece to client state. */
void add_piece_have(client_state state, void* piece, size_t piece_size);

/* Remove an owned piece from client state. */
void remove_piece_have(client_state state, void* piece);

/* Add a wanted piece to client state. */
void add_piece_want(client_state state, unsigned long hash);

/* Remove a wanted piece from client state. */
void remove_piece_want(client_state state, unsigned long hash);

/* Add a connected client. */
void add_client_connected(client_state state, connected_client client);

/* Remove a connected client. */
void remove_client_connected(client_state state, connected_client client);
