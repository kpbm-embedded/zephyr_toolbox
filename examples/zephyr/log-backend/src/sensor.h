#ifndef SENSOR_H
#define SENSOR_H
#include <stdint.h>
#include <stddef.h>

/* checks a reading and logs: DBG every value, WRN near the limit, ERR above it; returns 0 if in range */
int sensor_check(int32_t value, int32_t limit);
void sensor_dump(const uint8_t *frame, size_t len);

#endif
