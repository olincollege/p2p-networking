#include <cstddef>
#include <cstdint>
#include <stdint.h>
#include <stdlib.h>

#include "./hash_table.h"
#include "./state.h"

client_state new_state(void) {
  client_state new_state = {malloc(sizeof(new_state))};
  new_state.clients_connected = make_table();
  new_state.pieces_have = make_table();
  new_state.pieces_want = make_table();
  return new_state;
}

void dealloc_state(client_state *state) {
  hash_dealloc(&state->clients_connected);
  hash_dealloc(&state->pieces_have);
  hash_dealloc(&state->pieces_want);
}

void add_piece_want(client_state state, unsigned long hash) {
  set_value(&state.pieces_want, &hash, sizeof(unsigned long), NULL, 1);
}

void remove_piece_want(client_state state, unsigned long hash) {
  remove_kv_pair(&state.pieces_want, &hash, sizeof(unsigned long));
}

void add_client_connected(client_state state, connected_client client) {
  set_value(&state.clients_connected, &client, sizeof(struct connected_client),
            NULL, 1);
}

void remove_client_connected(client_state state, connected_client client) {
  remove_kv_pair(&state.clients_connected, &client,
                 sizeof(struct connected_client));
}
