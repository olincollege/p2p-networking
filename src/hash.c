#include <string.h> 

/* http://www.cse.yorku.ca/~oz/hash.html
 * A copy of the djb2 hash function by https://en.wikipedia.org/wiki/Daniel_J._Bernstein
 * It is not cryptographically secure, but is effective and fast in practice. 
*/
unsigned long djb2(unsigned char *str) {
    // NOLINTBEGIN -- this isn't our code makes no sense to change it for the linter
    unsigned long hash = 5381; 
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + (long unsigned int) c; /* hash * 33 + c */

    return hash;
    // NOLINTEND
}

// A version that takes in a generic array
unsigned long djb2_bytes(void* bytes, size_t num) {
    // NOLINTBEGIN
    unsigned long hash = 5381;
    char c = 0;
    for (size_t i = 0; i < num; i++) {
        // Treat bytes as an array of chars
        c = ((char *) bytes)[i];
        hash = ((hash << 5) + hash) + (unsigned long)c; /* hash * 33 + c */
    }

    return hash;
    // NOLINTEND
}
