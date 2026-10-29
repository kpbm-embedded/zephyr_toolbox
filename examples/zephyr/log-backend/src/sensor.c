#include <zephyr/logging/log.h>
#include "sensor.h"

/* compile-time level of this module: INF. LOG_DBG below is compiled out (no code, no string). */
LOG_MODULE_REGISTER(sensor, LOG_LEVEL_INF);

int sensor_check(int32_t value, int32_t limit)
{
	LOG_DBG("value %d", value);
	if (value > limit) {
		LOG_ERR("over range %d > %d", value, limit);
		return -1;
	}
	if (value > limit - limit / 10) {
		LOG_WRN("near limit %d", value);
	} else {
		LOG_INF("ok %d", value);
	}
	return 0;
}

void sensor_dump(const uint8_t *frame, size_t len)
{
	LOG_HEXDUMP_INF(frame, len, "frame");
}
