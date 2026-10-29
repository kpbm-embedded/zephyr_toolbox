#ifndef ZONES_H
#define ZONES_H
#include <stdbool.h>
#include <stdint.h>

struct zone {
	const char *name;
	int32_t setpoint;
	int32_t hysteresis;
	bool cooling;
};

/* one entry per enabled acme,thermostat node, built at compile time from the devicetree */
extern const struct zone zones[];
extern const int zone_count;

/* two-point control: returns the new output state (heater/cooler on) for a measured temperature */
bool zone_control(const struct zone *z, int32_t mdeg, bool on);

#endif
