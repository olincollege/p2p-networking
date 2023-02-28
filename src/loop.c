#include <arpa/inet.h>
#include <bits/getopt_core.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "./message.h"
#include "./network.h"

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
        accept(file_descriptor, (struct sockaddr *)&in_address,
               &adress_length); // NOLINT we have no accept4 on ubuntu
    if (accept_res < 0) {
      // no more connection left
      if (errno == EWOULDBLOCK || errno == EAGAIN) {
        puts("all connections accepted");
        break;
      }
      puts("error accepting connections");
      break;
    }
    // start monitoring the connection
    non_blocking_socket(accept_res);
    struct epoll_event client_connection;
    client_connection.data.u64 = as_epoll_data(accept_res, EPOLL_PEER_FD);
    client_connection.events = EPOLLIN | EPOLLET;

    // bind connection to epoll
    int epoll_bind =
        epoll_ctl(epoll_c, EPOLL_CTL_ADD, accept_res, &client_connection);
    if (epoll_bind < 0) {
      puts("failed to bind new connection to epoll container");
    }
  }
}

// main I/O loop for the program
void loop(int epoll_c, struct epoll_event *events) {
  int num_changes =
      epoll_wait(epoll_c, events, MAX_EPOLL_EVENTS, EPOLL_TIMEOUT);
  if (num_changes < 0) {
    puts("epoll_wait call failed");
    return;
  }
  printf("num changes %d\n", num_changes);
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

    } else {
      puts("faillthrough");
      return;
    }
  }
}

int main(int argc, char *argv[]) {

  /* Process CLI Arguments
   *
   * -S server mode, uses a fixed port
   */
  int is_server = 0; // 0 is client, 1 is server

  int arg = 0;
  while ((arg = getopt(argc, argv, "S")) != EOF) {
    // NOLINTBEGIN -- Switch is extendable to more CLI args. Linter doesn't
    //                like 1 arg switch though.
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

  int epoll_c = 0;
  if (is_server) {
    epoll_c = create_epoll_socket(SERVER_LISTEN_PORT);
  } else {
    epoll_c = create_epoll_socket(0);
  }
  struct epoll_event events[MAX_EPOLL_EVENTS];

  puts("running I/O loop.");
  while (1) {
    loop(epoll_c, events);
  }
}

void read_message(int file_descriptor) {
  int message_len = full_message_availiable(file_descriptor);

  if (message_len) {
    uint8_t message[MAX_SIZE_MESSAGE_INT*4];
    uint8_t message_type;
    // https://pubs.opengroup.org/onlinepubs/007904975/functions/recv.html
    // Peek the message at the socket.
    recv(file_descriptor, message, sizeof(message), 0);
    memcpy(message + 4, &message_type, 1);

    // Ask message
    if (message_type == 0) {
      struct ask_message message_read;
      memcpy(message, &message_read, message_len);
    } else if (message_type == 1) {
      struct give_message message_read;
      memcpy(message, &message_read, message_len);
    } else {
      // Allocate the space for the peer message's flexible array.
      struct peer_message *message_read = malloc(sizeof(message));
      memcpy(message, &message_read, message_len);
      free(message_read);
    }
  }
}
