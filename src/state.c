#include <openssl/sha.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "./hash_table.h"
#include "./message.h"
#include "./state.h"

client_state new_state(void) {
  client_state new_state;
  new_state.file_descriptors = make_table();
  new_state.ports = make_table();
  new_state.pieces_have = make_table();
  new_state.pieces_want = make_table();
  return new_state;
}

client_state demo_state() {
  // seed random generator
  srand((int)getpid());

  // parameters for our demo client
  const unsigned int MAX_PEICES = 100;
  const unsigned int HAVE_AMOUNT = 30;
  const unsigned int WANT_AMOUNT = 60;

  client_state state = new_state();
  while (state.pieces_have.num_elements != HAVE_AMOUNT) {
    uint8_t piece[PIECE_SIZE_BYTES];
    memset(piece, 0, sizeof(piece));
    piece[0] = (int)rand() % MAX_PEICES;
    add_piece_have(&state, piece, sizeof(piece));
  }

  return state;
}

void dealloc_state(client_state *state) {
  hash_dealloc(&state->file_descriptors);
  hash_dealloc(&state->ports);
  hash_dealloc(&state->pieces_have);
  hash_dealloc(&state->pieces_want);
}

void add_piece_have(client_state *state, void *piece, size_t piece_size) {
  unsigned char piece_hash[SHA256_DIGEST_LENGTH];
  SHA256(piece, piece_size, piece_hash);
  set_value(&state->pieces_have, piece_hash, SHA256_DIGEST_LENGTH, piece,
            piece_size);
}

void remove_piece_have(client_state *state, void *piece, size_t piece_size) {
  unsigned char piece_hash[SHA256_DIGEST_LENGTH];
  SHA256(piece, piece_size, piece_hash);
  remove_kv_pair(&state->pieces_have, piece_hash, SHA256_DIGEST_LENGTH);
}

void add_piece_want(client_state *state, unsigned char *hash) {
  set_value(&state->pieces_want, hash, SHA256_DIGEST_LENGTH, NULL, 1);
}

void remove_piece_want(client_state *state, unsigned char *hash) {
  remove_kv_pair(&state->pieces_want, hash, SHA256_DIGEST_LENGTH);
}

void add_file_descriptor(client_state *state, int file_descriptor) {
  set_value(&state->file_descriptors, &file_descriptor, sizeof(int), NULL, 1);
}

void remove_file_descriptor(client_state *state, int file_descriptor) {
  remove_kv_pair(&state->file_descriptors, &file_descriptor, sizeof(int));
}

void add_port(client_state *state, uint16_t port) {
  set_value(&state->ports, &port, sizeof(uint16_t), NULL, 1);
}

void remove_port(client_state *state, uint16_t port) {
  remove_kv_pair(&state->ports, &port, sizeof(uint16_t));
}
