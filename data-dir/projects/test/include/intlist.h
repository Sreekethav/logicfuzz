#ifndef INTLIST_H
#define INTLIST_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Parses a comma-separated list of integers, returns their sum or error codes
int parse_int_list(const char* s, size_t n);

#ifdef __cplusplus
}
#endif

#endif // INTLIST_H
