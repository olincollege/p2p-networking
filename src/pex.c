#include <stdio.h> 
#include <stdlib.h> 

#include <arpa/inet.h> 
#include <sys/socket.h>
#include <sys/epoll.h>

void create_server() {
    const struct sockaddr_in server_adress = {
        AF_INET6,                       // use ipv6 resolution
        (sa_family_t) 8201,             // use port 8200  
        inet_addr("127.0.0.1")          // listen on localhost
    };

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

    // specify 50 maximum backlog of un-accepted connections
    // this dosn't actually do anything, just marks an attribute  
    listen(server_socket, 50); 
    
    // create an epoll container for multiplexing I/O      
    // https://man7.org/linux/man-pages/man2/epoll_create.2.html
    int ep = epoll_create(100);

}


int main() {
    puts("hi!:)");    
}
