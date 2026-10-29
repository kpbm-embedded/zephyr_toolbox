#include <errno.h>
#include "avg.h"
#include "sensor_hw.h"

int sensor_avg(int n, int16_t *out)
{
	int32_t sum = 0;
	int good = 0, errors = 0;
	int16_t raw;

	while (good < n) {
		if (sensor_hw_read(&raw) == 0) {
			sum += raw;
			good++;
		} else if (++errors >= n) {
			return -EIO;
		}
	}
	*out = (int16_t)(sum / n);
	return 0;
}
