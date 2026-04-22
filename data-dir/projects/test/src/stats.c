#include "stats.h"

int compute_average(const int* values, size_t count, double* out_avg) {
    if (!values || count == 0 || !out_avg) return -1;

    long long sum = 0;
    for (size_t i = 0; i < count; i++) {
        sum += values[i];
    }

    *out_avg = (double)sum / count;
    return 0;
}
