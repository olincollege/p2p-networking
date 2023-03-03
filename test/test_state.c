#include <criterion/assert.h>
#include <criterion/criterion.h>
#include <criterion/internal/new_asserts.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>
#include <fcntl.h>
#include <stdint.h>
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

    // recover the written message
    char* buffer = malloc(1024*1024);
    size_t bytes_read = 0;
    FILE* fd_read = fopen("/tmp/test_peer_exchange", "r");
    while(1) {
        char ch = fgetc(fd_read);
        if(feof(fd_read)) {
            break;
        }
        buffer[bytes_read] = ch;
        bytes_read++;
    }
    fclose(fd_read);
    peer_message* message = malloc(bytes_read);
    memcpy(message, buffer, bytes_read);

    // assert against the written message
    cr_assert(eq(int, sizeof(peer_message) + 4*sizeof(peer_info), bytes_read));
    cr_assert(eq(int, sizeof(peer_message) + 4*sizeof(peer_info) - sizeof(message->message_size), (int)message->message_size));
    cr_assert(eq(int, 2, (int)message->type));

    in_port_t port_1 = message->peers[0].addr_port;
    in_port_t port_2 = message->peers[1].addr_port;
    in_port_t port_3 = message->peers[2].addr_port;
    in_port_t port_4 = message->peers[3].addr_port;


    // ordering not guaranteed by the hash_map
    cr_assert(eq(int, 1, port_1 == 1 || port_2 == 1 || port_3 == 1 || port_4 == 1));
    cr_assert(eq(int, 1, port_1 == 2 || port_2 == 2 || port_3 == 2 || port_4 == 2));
    cr_assert(eq(int, 1, port_1 == 3 || port_2 == 3 || port_3 == 3 || port_4 == 3));
    cr_assert(eq(int, 1, port_1 == 4 || port_2 == 4 || port_3 == 4 || port_4 == 4));

    // cleanup
    free(message);
    free(buffer);
    dealloc_state(&state);
}

Test(test_state, test_broadcast_want_1piece1client) {
    remove("/tmp/test_broadcast_want");
    int fd_in = open("/tmp/test_broadcast_want",
                     O_CREAT | O_APPEND | O_RDWR, S_IRUSR | S_IWUSR);
    if(fd_in < 0) {
        puts("failed to open test_state fd");
    }

    // say we are looking for 1 piece (256bit "hash")
    uint64_t send_hash[4] = {1, 2, 3, 4};
    client_state state = new_state();
    add_piece_want(&state, send_hash);

    // say we are connected to a "client" mocked as stdin
    add_file_descriptor(&state, fd_in);

    // attempt to broadcast to fds
    broadcast_want(&state);
    close(fd_in);

    // recover the written message
    char* buffer = malloc(1024*1024);
    size_t bytes_read = 0;
    FILE* fd_read = fopen("/tmp/test_broadcast_want", "r");
    while(1) {
        char ch = fgetc(fd_read);
        if(feof(fd_read)) {
            break;
        }
        buffer[bytes_read] = ch;
        bytes_read++;
    }
    fclose(fd_read);

    // assert written message is the correct size
    cr_assert(eq(int, bytes_read, sizeof(ask_message)),
              "Sent message is not expected size.");

    // Recover the message
    ask_message message;
    memcpy(&message, buffer, bytes_read);

    // assert against written message
    // message_size field
    cr_assert(eq(int,
                 message.message_size,
                 sizeof(ask_message)-sizeof(uint32_t)));
    // type field
    cr_assert(eq(int, message.type, 0));
    // sha256 field (we sent the array {1, 2, 3, 4})
    puts(message.sha256[0]);
    puts(message.sha256[1]);
    puts(message.sha256[2]);
    puts(message.sha256[3]);
    cr_assert(eq(int, message.sha256[0], 1));
    cr_assert(eq(int, message.sha256[1], 2));
    cr_assert(eq(int, message.sha256[2], 3));
    cr_assert(eq(int, message.sha256[3], 4));

    // cleanup
    free(buffer);
    dealloc_state(&state);
}

// NOLINTEND
