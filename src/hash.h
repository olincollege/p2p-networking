#pragma once

#include <string.h> 

/* http://www.cse.yorku.ca/~oz/hash.html
 * A copy of the djb2 hash function by https://en.wikipedia.org/wiki/Daniel_J._Bernstein
 * It is not cryptographically secure, but is effective and fast in practice. 
*/
unsigned long djb2(unsigned char *str); 

unsigned long djb2_bytes(void* bytes, size_t num);
