#include <arpa/inet.h>
#include <bits/getopt_core.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "./loop.h"
#include "./message.h"
#include "./network.h"
#include "./state.h"

/* When epoll notifies us of a connnection of our socket, we want to accept all
   of them and add them to our listening queue.
 */
void on_connection(int epoll_c, int file_descriptor) {
  puts("attempting to accept connection");
  // accept all possible connections
  while (1) {
    struct sockaddr_in6 in_address;
    socklen_t adress_length = sizeof(struct sockaddr);
    int accept_res =
        accept(file_descriptor, (struct sockaddr *)&in_address, // NOLINT
               &adress_length);
    if (accept_res < 0) {
      // no more connection left
      if (errno == EWOULDBLOCK || errno == EAGAIN) {
        puts("all connections accepted");
        break;
      }
      puts("error accepting connections");
      break;
    }
    if (bind_epoll(epoll_c, accept_res) < 0) {
      puts("failed to bind new connection to epoll container");
    }
  }
}

// main I/O loop for the program
void loop(int epoll_c, struct epoll_event *events, client_state* state) {
  int num_changes =
      epoll_wait(epoll_c, events, MAX_EPOLL_EVENTS, EPOLL_TIMEOUT);
  if (num_changes < 0) {
    puts("epoll_wait call failed");
    return;
  }
  for (size_t i = 0; i < (size_t)num_changes; i++) {
    struct epoll_event epoll_e = events[i];
    int file_descriptor = as_custom_data(events[i].data.u64).fd;
    int event_type = as_custom_data(events[i].data.u64).type;

    printf("processing, fd: %d, et: %d \n", file_descriptor, event_type);

    if (epoll_e.events & EPOLLERR) { // NOLINT expected usage
      puts("error on epoll event");
      close(file_descriptor);
    } else if (epoll_e.events & EPOLLHUP) { // NOLINT expected usage
      puts("holdup error on epoll event");
      close(file_descriptor);
    }
    // we have a pending connection on our socket
    else if (event_type == EPOLL_LISTEN_FD) {
      on_connection(epoll_c, file_descriptor);
    }
    // we have data on our socket
    else if (event_type == EPOLL_PEER_FD) {
      read_message(file_descriptor, epoll_c, state);
    } else {
      puts("faillthrough error");
      return;
    }
  }
}

int main(int argc, char *argv[]) {
   /* Initialize a demo state by randomly choosing have/want pieces */
   const unsigned int SEED_MAX_PEICES = 100; 
   const unsigned int SEED_HAVE_AMOUNT = 30;
   const unsigned int SEED_WANT_AMOUNT = 30;
   client_state state = demo_state(SEED_MAX_PEICES, SEED_HAVE_AMOUNT, SEED_WANT_AMOUNT);

  /* Process CLI Arguments
   *
   * -S server mode, uses a fixed port
   */
  int is_server = 0; // 0 is client, 1 is server

  int arg = 0;
  // NOLINTBEGIN -- Switch is extendable to more CLI args. Linter doesn't
  //                like 1 arg switch though.
  while ((arg = getopt(argc, argv, "S")) != EOF) {
    switch (arg) {
    case 'S':
      is_server = 1;
      break;
    }
    // NOLINTEND
  }

  // NOLINTBEGIN -- Keeping this in case we ever want to read positional
  //                arguments. Linter does not like that these values are never
  //                read or used.
  argc -= optind;
  argv += optind;
  // NOLINTEND

  // Begin the actual program

  our_server server = create_epoll_socket(is_server ? SERVER_LISTEN_PORT : 0);
  int epoll_c = server.file_descriptor;
  add_port(&state, server.port); // add ourselves to the peer broadcast

    
  if(!is_server) {
      // bootstrap to known server
      struct sockaddr_in6 bootstrap_addr; 
      memset(&bootstrap_addr, '\0', sizeof(bootstrap_addr)); // NOLINT
      bootstrap_addr.sin6_family = AF_INET6;                 // use ipv6 resolution
      bootstrap_addr.sin6_port = htons(SERVER_LISTEN_PORT);  // port to connect on
      inet_pton(AF_INET6, "::1", &bootstrap_addr.sin6_addr); // connect on localhost
      peer_info bootstrap = {bootstrap_addr.sin6_addr, SERVER_LISTEN_PORT};
      int peer_fd = connect_to_peer(bootstrap, epoll_c);
      if(!peer_fd) {
        puts("failed to connect to the bootstrap server!");
        exit(1); // NOLINT
      }
      add_file_descriptor(&state, peer_fd);
      puts("added bootstrap to listen queue!");
  }
  else {
     puts("skipping bootstrapping because we are the bootstrap node");
  }
  

  struct epoll_event events[MAX_EPOLL_EVENTS];

  printf("running I/O loop on port %d!\n", (int)server.port);
  size_t iter = 0;
  const size_t BROADCAST_TIMEOUT = 10; // broadcast every 10sec
  while (1) {
    iter++;
    if(!(iter % BROADCAST_TIMEOUT)) {
       puts("starting broadcast of want pieces");
       broadcast_want(&state);
       puts("starting broadcast of peer exchange");
       peer_exchange(&state);
    }
    loop(epoll_c, events, &state);
  }
}

void read_message(int file_descriptor, int epoll_fd, client_state *state) {
  int message_len = full_message_availiable(file_descriptor);

  if (message_len) {
    uint8_t message[MAX_SIZE_MESSAGE_INT * 4];
    uint8_t message_type = 0;
    // https://pubs.opengroup.org/onlinepubs/007904975/functions/recv.html
    // Peek the message at the socket.
    recv(file_descriptor, message, sizeof(message), 0);
    memcpy(message + 4, &message_type, 1); // NOLINT

    // Ask message
    if (message_type == 0) {
      struct ask_message message_read;
      memcpy(&message_read, &message, message_len); // NOLINT
      send_if_have(state, message_read, file_descriptor);
    } else if (message_type == 1) {
      struct give_message message_read;
      memcpy(&message_read, &message, message_len); // NOLINT
      add_piece_have(state, &(message_read.piece), PIECE_SIZE_BYTES);
    } else {
      // Allocate the space for the peer message's flexible array.
      struct peer_message *message_read = malloc(message_len); // NOLINT
      memcpy(&message_read, message, message_len);             // NOLINT
      size_t num_peers = (message_len - sizeof(peer_message)) / sizeof(peer_info);
      connect_to_list(message_read->peers, num_peers, state, epoll_fd);
      free(message_read);
    }
  }
}



// Connect to a peer
// Returns file descriptor if successful, 0 if failed
int connect_to_peer(peer_info peer, int epoll_c) {
  printf("attemping connection to port: %d\n", (int)peer.addr_port);
  struct sockaddr_in6 in_address;
  memset(&in_address, '\0', sizeof(in_address)); // NOLINT
  in_address.sin6_family = AF_INET6;            // use ipv6 resolution
  in_address.sin6_port = htons(peer.addr_port); // port to connect on
  in_address.sin6_addr = peer.sin6_addr;        // IP to connect to.

  int to_connect = socket(AF_INET6, SOCK_STREAM, 0); // Create socket

  // Try connecting
  if (connect(to_connect, (const struct sockaddr *)&in_address, sizeof(in_address)) != -1) {
    if (bind_epoll(epoll_c, to_connect) < 0) {
      puts("failed to bind new connection to epoll container");
      return 0;
    }
    printf("connected to peer at port: %d\n", (int)peer.addr_port);
    return to_connect;
  }
  printf("failed to connect to peer at port: %d\n", (int)peer.addr_port);
  return 0;
}

// Connect to an array of peer
void connect_to_list(peer_info *peer_list, size_t n, client_state *state,
                     int epoll_c) {
  for (size_t peer = 0; peer < n; peer++) {
    if (get_kv_pair(&(state->ports), &(peer_list->addr_port),
                    sizeof(peer_info)) == NULL) {
      int new_connection = connect_to_peer(*peer_list, epoll_c);
      if (new_connection) {
        add_port(state, peer_list->addr_port);
        add_file_descriptor(state, new_connection);
      }
    }
    peer_list++; 
  }
}

// Bind a file descriptor to an epoll socket.
int bind_epoll(int epoll_c, int file_descriptor)  {
  // start monitoring the connection
    non_blocking_socket(file_descriptor);
    struct epoll_event client_connection;
    client_connection.data.u64 = as_epoll_data(file_descriptor, EPOLL_PEER_FD);
    client_connection.events = EPOLLIN | EPOLLET;

    // bind connection to epoll
    int epoll_bind =
        epoll_ctl(epoll_c, EPOLL_CTL_ADD, file_descriptor, &client_connection);
    return epoll_bind;
}
