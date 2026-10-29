#include <zephyr/drivers/sensor.h>
#include "thermal.h"

enum thermal_state thermal_check(const struct device *dev, int32_t trip_mdeg, int32_t *mdeg)
{
	struct sensor_value v;

	if (!device_is_ready(dev) || sensor_sample_fetch(dev) != 0 ||
	    sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &v) != 0) {
		return THERMAL_ERROR;
	}
	*mdeg = v.val1 * 1000 + v.val2 / 1000;
	if (*mdeg >= trip_mdeg) {
		return THERMAL_TRIP;
	}
	return *mdeg >= trip_mdeg - 5000 ? THERMAL_WARN : THERMAL_OK;
}
