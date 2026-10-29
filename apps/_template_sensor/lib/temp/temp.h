#ifndef TEMP_H
#define TEMP_H
#include <zephyr/device.h>

#define TEMP_AVG_N 4

struct temp_avg {
	int32_t v[TEMP_AVG_N];
	int n;   /* values so far (up to TEMP_AVG_N) */
	int pos;
};

int temp_read_mdeg(const struct device *dev, int32_t *mdeg);
int32_t temp_avg_add(struct temp_avg *a, int32_t mdeg); /* returns the average of the last N values */

#endif
