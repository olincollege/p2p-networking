#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <sys/epoll.h>

// Constants for our networking configuration
enum {
    MAX_EPOLL_EVENTS = 50,
    MAX_LISTEN_BACKLOG = 50,
    LISTEN_PORT = 8201,
    EPOLL_LISTEN_FD = 1,  // mark epoll events for our socket with this code
    EPOLL_PEER_FD = 2,   // mark epoll events for peer connections with this code
};

/* Makes a fcntl system call to mark a socket a non-blocking
 * We want to be able to handle multiple IO operations at the same time with epoll
 * https://man7.org/linux/man-pages/man2/fcntl.2.html
 */
void non_blocking_socket(int socket); 

/* Creates a TCP socket and binds it to port */
int create_socket(); 

/* Create an epoll container for the TCP listening socket.
 * We can use it to drive an I/O loop with many different types of file descriptors.
 */
int create_epoll_socket(); 
