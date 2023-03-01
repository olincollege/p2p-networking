#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>


#include "../src/message.h"
#include "../src/state.h"

// NOLINTBEGIN -- the magic constant warnings are useless here
Test(test_state, test_random_state) {
    const unsigned int MAX_PIECES = 100; 
    const unsigned int HAVE_AMOUNT = 30; 
    const unsigned int WANT_AMOUNT = 60;
    client_state state = demo_state(MAX_PIECES, HAVE_AMOUNT, WANT_AMOUNT);

    cr_assert(eq(int, HAVE_AMOUNT, state.pieces_have.num_elements));
    cr_assert(eq(int, WANT_AMOUNT, state.pieces_want.num_elements));
}

Test(test_state, test_peer_exhange) {
   remove("/tmp/test_peer_exchange");
   int fd_in = open("/tmp/test_peer_exchange", O_CREAT | O_APPEND | O_RDWR, S_IRUSR | S_IWUSR);    
   if(fd_in < 0) {
        puts("failed to open test_state fd");
   }

   // say we know about 4 random ports
   client_state state = new_state();
   add_port(&state, 1);
   add_port(&state, 2);
   add_port(&state, 3);
   add_port(&state, 4);

   // say we are connected to a "client" mocked as stdin
   add_file_descriptor(&state, fd_in);

   // attempt to broadcast to fds
   peer_exchange(&state);
   close(fd_in);

   // assert against the written message
   char* buffer = malloc(1024*1024); 
   size_t bytes_read = 0; 
   FILE* fd_read = fopen("/tmp/test_peer_exchange", "r");
   while(!feof(fd_read)) {
     char ch = fgetc(fd_read); 
     buffer[bytes_read] = ch;
     bytes_read++;
   }
   fclose(fd_read);
   peer_message* message = malloc(bytes_read);
   memcpy(message, buffer, bytes_read);

   free(message);
   free(buffer);
}

// NOLINTEND
