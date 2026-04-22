#ifndef STATS_H
#define STATS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Computes the average of an integer array.
// Returns 0 on success, -1 if input is invalid.
// Result is written to out_avg.
int compute_average(const int* values, size_t count, double* out_avg);

#ifdef __cplusplus
}
#endif

#endif // STATS_H
