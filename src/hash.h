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

