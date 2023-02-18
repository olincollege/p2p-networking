#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>


#include "./network.h"


/* When epoll notifies us of a connnection of our socket, we want to accept all of them 
   and add them to our listening queue.
 */
void on_connection(int epoll_c, int file_descriptor) {
    puts("attempting to accept connection");
    // accept all possible connections 
    while(1) {
        struct sockaddr_in6 in_address; 
        socklen_t adress_length = sizeof(struct sockaddr);
        int accept_res = accept(file_descriptor, (struct sockaddr*) &in_address, &adress_length);
        if(accept_res < 0) {
            // no more connection left
            if(errno == EWOULDBLOCK || errno == EAGAIN) {
                puts("all connections accepted"); 
                break;
            }
            // failed to accept
            else {
                puts("error accepting connections");
                break;
            }
        }
        else {
            // start monitoring the connection
            non_blocking_socket(accept_res);                
            struct epoll_event client_connection;
            client_connection.data.fd = as_epoll_data(accept_res, EPOLL_PEER_FD);
            client_connection.events = EPOLLIN | EPOLLET; 

            // bind connection to epoll
            int epoll_bind = epoll_ctl(epoll_c, EPOLL_CTL_ADD, accept_res, &client_connection);
        }
    }
}



// main I/O loop for the program
void loop(int epoll_c, struct epoll_event* events) {
    int num_changes = epoll_wait(epoll_c, events, MAX_EPOLL_EVENTS, EPOLL_TIMEOUT); 
    printf("num changes %d", num_changes);
    for(size_t i = 0; i < num_changes; i++) {
        struct epoll_event epoll_e = events[i];     
        int file_descriptor = as_custom_data(events[i].data.fd).fd;
        int event_type = as_custom_data(events[i].data.fd).type;

        if(epoll_e.events & EPOLLERR) {
            puts("error on epoll event");
            close(file_descriptor);
        }
        else if(epoll_e.events & EPOLLHUP) {
            puts("holdup error on epoll event");
            close(file_descriptor);
        }
        // we have a pending connection on our socket
        else if(event_type == EPOLL_LISTEN_FD) {
            on_connection(epoll_c, file_descriptor);
        }
        // we have data on our socket
        else if(event_type == EPOLL_PEER_FD) {
            
        }
    }
}

int main() {
    int epoll_c = create_epoll_socket();
    struct epoll_event* events;
    events = calloc (MAX_EPOLL_EVENTS, sizeof events);

    puts("running I/O loop");    
    while(1) {
        loop(epoll_c, events);
    }
}
