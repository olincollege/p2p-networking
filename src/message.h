#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

/* Our slides have more info on how messages are encoded
 * The first 4 bytes of a message describe how long the message is
 * The the next byte describes the type of message "ask", "give", or "peer list"
 * The remaining bytes are specific the type of the message
 */

// we need to serialize these structs over the wire
// we want to prevent the compiler from repacking or re-alligning the struct
// fields
#pragma pack(push, 1)

enum { PIECE_SIZE_BYTES = (1024 * 1024) / 8 };

// a struct for describing the info needed to address a peer
typedef struct peer_info {
  struct in6_addr sin6_addr; // 128 bit ipv6 adress
  in_port_t addr_port;       // uint16 for port number
} peer_info;

// a struct for describing the info needed to send a peer list
// uses the "flexible array" C11 concept (formerly known as "struct hack")
typedef struct peer_message {
  uint32_t message_size; // sizeof(peer_message) - 4
  uint8_t type;          // should be set to 2
  peer_info peers[];     // flexible size
} peer_message;

// a struct for describing the info needed to ask for a 1MB piece
typedef struct ask_message {
  uint32_t message_size; // sizeof(ask_message) - 4
  uint8_t type;          // should be set to 0
  uint64_t sha256[4];
} ask_message;

// a struct for describing the info needed to send a 1MB piece
typedef struct give_message {
  uint32_t message_size; // sizeof(give_message) - 4
  uint8_t type;          // should be set to 1
  uint64_t sha256[4];
  uint8_t piece[PIECE_SIZE_BYTES]; // 1 MiB
} give_message;

#pragma pack(pop)
