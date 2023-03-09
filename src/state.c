#include <arpa/inet.h>
#include <openssl/sha.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
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

static unsigned rand_no_lint(void) {
  return (unsigned)(rand()); // NOLINT(cert-msc30-c,cert-msc50-cpp,concurrency-mt-unsafe)
}

client_state demo_state(const unsigned int max_peices,
                        const unsigned int have_amount,
                        const unsigned int want_amount) {

  client_state state = new_state();

  // seed random generator
  srand((unsigned int)getpid());

  while (state.pieces_have.num_elements != have_amount) {
    uint32_t piece[PIECE_SIZE_BYTES / 4] = {0};
    piece[0] = rand_no_lint() % max_peices;
    add_piece_have(&state, piece, sizeof(piece));
  }

  while (state.pieces_want.num_elements != want_amount) {
    uint32_t piece[PIECE_SIZE_BYTES / 4] = {0};
    piece[0] = rand_no_lint() % max_peices;
    unsigned char piece_hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)piece, sizeof(piece), piece_hash);
    add_piece_want(&state, piece_hash);
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
  remove_piece_want(state, piece_hash);
}

void remove_piece_have(client_state *state, void *piece, size_t piece_size) {
  unsigned char piece_hash[SHA256_DIGEST_LENGTH];
  SHA256(piece, piece_size, piece_hash);
  remove_kv_pair(&state->pieces_have, piece_hash, SHA256_DIGEST_LENGTH);
}

void add_piece_want(client_state *state, unsigned char *hash) {
  int empty = 0;
  set_value(&state->pieces_want, hash, SHA256_DIGEST_LENGTH, &empty,
            sizeof(empty));
}

void remove_piece_want(client_state *state, unsigned char *hash) {
  remove_kv_pair(&state->pieces_want, hash, SHA256_DIGEST_LENGTH);
}

void add_file_descriptor(client_state *state, int file_descriptor) {
  int empty = 0;
  set_value(&state->file_descriptors, &file_descriptor, sizeof(int), &empty,
            sizeof(empty));
}

void remove_file_descriptor(client_state *state, int file_descriptor) {
  remove_kv_pair(&state->file_descriptors, &file_descriptor, sizeof(int));
}

void add_port(client_state *state, uint16_t port) {
  int empty = 0;
  set_value(&state->ports, &port, sizeof(uint16_t), &empty, sizeof(empty));
}

void remove_port(client_state *state, uint16_t port) {
  remove_kv_pair(&state->ports, &port, sizeof(uint16_t));
}

void send_if_have(client_state *state, ask_message message, int peer) {
  kv_pair* piece = get_kv_pair(&state->pieces_have, message.sha256, sizeof(message.sha256));  give_message send_message;
  if(piece) {
    send_message.message_size = GIVE_MESSAGE_SIZE;
    send_message.type = GIVE_MESSAGE_TYPE;
    memcpy(send_message.sha256, message.sha256, sizeof(message.sha256)); //NOLINT
    memcpy(send_message.piece, piece->value, PIECE_SIZE_BYTES); //NOLINT
    write(peer, &send_message, sizeof(send_message));
  }
}

void peer_exchange(client_state *state) {
  vector_kv_pair clients_connected = collect_table(&state->file_descriptors);
  vector_kv_pair known_ports = collect_table(&state->ports);

  // craft the peer message
  size_t peer_message_size =
      sizeof(peer_message) + sizeof(peer_info) * known_ports.size;
  peer_message *message = malloc(peer_message_size);
  message->message_size =
      (uint32_t)(peer_message_size - sizeof(message->message_size));
  message->type = 2;
  for (size_t port = 0; port < known_ports.size; port++) {
    // Pack known peers into the message
    struct sockaddr_in6 addr;
    inet_pton(AF_INET6, "::1", &addr.sin6_addr);
    peer_info info = {addr.sin6_addr, *(in_port_t *)known_ports.arr[port].key};
    message->peers[port] = info;
  }

  // send the peer message
  for (size_t peer = 0; peer < clients_connected.size; peer++) {
    int peer_fd = *(int *)clients_connected.arr[peer].key;
    ssize_t send_res = write(peer_fd, message, peer_message_size);
    printf("starting to write to peer\n");
    if (send_res < 0) {
      printf("failed to write peer list, closing fd: %d\n, err: %d", peer_fd,
             (int)send_res);
      close(peer_fd);
    } else {
      printf("sent peer list to fd: %d\n", peer_fd);
    }
  }

  // cleanup
  free_vec_kv_pair(&clients_connected);
  free_vec_kv_pair(&known_ports);
  free(message);
}

void broadcast_want(client_state *state) {
  vector_kv_pair clients_connected = collect_table(&state->file_descriptors);
  vector_kv_pair pieces_wanted = collect_table(&state->pieces_want);

  // Craft the ask message
  ask_message message;
  message.message_size = (uint32_t)ASK_MESSAGE_SIZE;
  message.type = (uint8_t)ASK_MESSAGE_TYPE;
  // Nested for loops because we designed our protocol to only send 1 hash at a
  // time
  for (size_t hashnum = 0; hashnum < pieces_wanted.size; hashnum++) {
    // Pack hash into message
    memcpy(message.sha256, pieces_wanted.arr[hashnum].key, // NOLINT
           HASH_SIZE);

    // Send the want message to each peer
    for (size_t peer = 0; peer < clients_connected.size; peer++) {
      int peer_fd = *(int *)clients_connected.arr[peer].key;
      printf("starting to write to peer\n");
      ssize_t send_res = write(peer_fd, &message, sizeof(ask_message));
      if (send_res < 0) {
        printf("failed to write ask message, closing fd: %d\n, err: %d",
               peer_fd, (int)send_res);
        close(peer_fd);
      } else {
        printf("sent want piece message to fd: %d\n", peer_fd);
      }
    }
  }

  // cleanup
  free_vec_kv_pair(&clients_connected);
  free_vec_kv_pair(&pieces_wanted);
}
