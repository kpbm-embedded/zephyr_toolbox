#ifndef THERMAL_H
#define THERMAL_H
#include <zephyr/device.h>

enum thermal_state { THERMAL_OK, THERMAL_WARN, THERMAL_TRIP, THERMAL_ERROR };

/* reads the local temperature of a sensor device (any driver with SENSOR_CHAN_AMBIENT_TEMP) and
 * classifies it: >= trip_mdeg TRIP, >= trip_mdeg - 5000 WARN, else OK
 */
enum thermal_state thermal_check(const struct device *dev, int32_t trip_mdeg, int32_t *mdeg);

#endif
