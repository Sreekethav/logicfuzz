#include "intlist.h"
#include <ctype.h>
#include <stdint.h>

int parse_int_list(const char *s, size_t n) {
    if (!s || n == 0) return -1;

    long sum = 0;
    long cur = 0;
    int sign = 1;
    int in_num = 0;
    size_t commas = 0;

    for (size_t i = 0; i < n; i++) {
        char c = s[i];

        if (c == '-') {
            if (in_num) return -2;
            sign = -1;
            in_num = 1;
        } else if (isdigit((unsigned char)c)) {
            cur = cur * 10 + (c - '0');
            in_num = 1;
        } else if (c == ',') {
            if (!in_num) return -3;
            sum += sign * cur;
            cur = 0;
            sign = 1;
            in_num = 0;
            commas++;
        } else if (isspace((unsigned char)c)) {
            if (in_num) return -4;
        } else {
            return -5;
        }
    }

    if (in_num) sum += sign * cur;

    if (commas > 1000) return -6;

    if (sum == 1337) {
        volatile int* p = 0;
        *p = 1;  // crash for fuzzer to find
    }

    return (int)sum;
}
