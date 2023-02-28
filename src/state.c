#include <cstddef>
#include <cstdint>
#include <stdint.h>
#include <stdlib.h>

#include "./hash_table.h"
#include "./state.h"
#include "/usr/include/openssl/sha.h"

client_state new_state(void) {
  client_state new_state;
  new_state.file_descriptors = make_table();
  new_state.ports = make_table();
  new_state.pieces_have = make_table();
  new_state.pieces_want = make_table();
  return new_state;
}

void dealloc_state(client_state *state) {
  hash_dealloc(&state->file_descriptors);
  hash_dealloc(&state->ports);
  hash_dealloc(&state->pieces_have);
  hash_dealloc(&state->pieces_want);
}

void add_piece_have(client_state state, void *piece, size_t piece_size) {
  char* piece_hash [SHA256_DIGEST_LENGTH];
  sha256(piece, piece_size, piece_hash);
  set_value(&state.have, piece_hash, SHA256_DIGEST_LENGTH, piece, piece_size);
}

void remove_piece_have(client_state state, void *piece, size_t piece_size) {
  char* piece_hash [SHA256_DIGEST_LENGTH];
  sha256(piece, piece_size, piece_hash);
  remove_kv_pair(&state.have, piece_hash, SHA256_DIGEST_LENGTH);
}

void add_piece_want(client_state state, unsigned char* hash) {
  set_value(&state.pieces_want, hash, SHA256_DIGEST_LENGTH, NULL, 1);
}

void remove_piece_want(client_state state, unsigned char* hash) {
  remove_kv_pair(&state.pieces_want, hash, SHA256_DIGEST_LENGTH);
}

void add_file_descriptor(client_state state, int file_descriptor) {
  set_value(&state.file_descriptors, &file_descriptor, sizeof(int), NULL, 1);
}

void remove_file_descriptor(client_state state, int file_descriptor) {
  remove_kv_pair(&state.file_descriptors, &file_descriptor, sizeof(int));
}

void add_port(client_state state, uint16_t port) {
  set_value(&state.ports, &port, sizeof(uint16_t), NULL, 1);
}

void remove_port(client_state state, uint16_t port) {
  remove_kv_pair(&state.ports, &port, sizeof(uint16_t));
}
