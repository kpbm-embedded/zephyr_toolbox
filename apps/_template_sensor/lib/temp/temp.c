#include <zephyr/drivers/sensor.h>
#include "temp.h"

int temp_read_mdeg(const struct device *dev, int32_t *mdeg)
{
	struct sensor_value v;
	int err;

	if (!device_is_ready(dev)) {
		return -ENODEV;
	}
	err = sensor_sample_fetch_chan(dev, SENSOR_CHAN_AMBIENT_TEMP);
	if (!err) {
		err = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &v);
	}
	if (!err) {
		*mdeg = (int32_t)(sensor_value_to_milli(&v));
	}
	return err;
}

int32_t temp_avg_add(struct temp_avg *a, int32_t mdeg)
{
	int64_t sum = 0;

	a->v[a->pos] = mdeg;
	a->pos = (a->pos + 1) % TEMP_AVG_N;
	if (a->n < TEMP_AVG_N) {
		a->n++;
	}
	for (int i = 0; i < a->n; i++) {
		sum += a->v[i];
	}
	return (int32_t)(sum / a->n);
}
