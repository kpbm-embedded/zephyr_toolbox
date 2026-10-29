#ifndef AVG_H
#define AVG_H
#include <stdint.h>

/* Average of n sensor reads; read errors are retried up to n times in total. 0 or -EIO. */
int sensor_avg(int n, int16_t *out);
#endif
