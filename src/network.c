#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <sys/epoll.h>
#include "./network.h"

uint64_t as_epoll_data(int32_t fd, int32_t type) {
    epoll_custom_data event_d = {fd, type};
    uint64_t result;  
    memcpy(&result, &event_d, sizeof(uint64_t));
    return result;
}
epoll_custom_data as_custom_data(uint64_t epoll_data_result) {
    epoll_custom_data result = {(int)0, (int)0};
    memcpy(&result, &epoll_data_result, sizeof(uint64_t));
    return result;
}

/* Makes a fcntl system call to mark a socket a non-blocking
 * We want to be able to handle multiple IO operations at the same time with epoll
 * https://man7.org/linux/man-pages/man2/fcntl.2.html
 */
void non_blocking_socket(int socket) {
    int flags = fcntl(socket, F_GETFL);  
    if(flags < 0) {
        puts("failed to retrieve socket flags");
        exit(1);
    }
    int set_flag = fcntl(socket, F_SETFL, flags | O_NONBLOCK);
    if(set_flag < 0) {
        puts("failed to set socket flag");
        exit(1);
    }
}

/* Creates a TCP socket and binds it to port */
int create_socket() {
    struct sockaddr_in6 server_adress = {
        AF_INET6,                       // use ipv6 resolution
        htons(LISTEN_PORT),             // port to listen on  
        0,                              // null pad for inet_pton command
    }; 
    inet_pton(AF_INET6, "::1", &server_adress.sin6_addr); // listen on localhost

    // try to allocate a TCP socket from OS
    int server_socket = socket(AF_INET6, SOCK_STREAM, 0);
    if(server_socket < 0) {
        puts("failed to allocate socket");
        exit(1);
    }

    // try to bind socket to port 
    int bind_res = bind(server_socket, (const struct sockaddr*) &server_adress, sizeof(server_adress));
    if(bind_res < 0) {
        puts("failed to bind to socket"); 
        exit(1);
    }

    // specify maximum backlog of un-accepted connections
    // this dosn't actually do anything, just marks an attribute  
    listen(server_socket, MAX_LISTEN_BACKLOG); 

    return server_socket;
}

/* Create an epoll container for the TCP listening socket.
 * We can use it to drive an I/O loop with many different types of file descriptors.
 */
int create_epoll_socket() {
    // create a socket for our server
    int server_socket = create_socket(); 
    non_blocking_socket(server_socket);


    // create an epoll container for multiplexing I/O      
    // https://man7.org/linux/man-pages/man2/epoll_create.2.html
    int epoll_descriptor = epoll_create1(0);
    if(epoll_descriptor < 0) {
        puts("failed to create epoll descriptor");
        exit(1);
    }

    // wrap server socket into an epoll event 
    struct epoll_event server_epoll; 
    server_epoll.data.u64 = as_epoll_data(server_socket, EPOLL_LISTEN_FD); 

    // https://man7.org/linux/man-pages/man7/epoll.7.html
    // mark as edge-triggering with I/O in
    server_epoll.events = EPOLLIN | EPOLLET; 
    if(epoll_ctl(epoll_descriptor, EPOLL_CTL_ADD, server_socket, &server_epoll) < 0) {
        puts("failed to bind socket to epoll descriptor");
        exit(1);
    }

    return epoll_descriptor;
}
