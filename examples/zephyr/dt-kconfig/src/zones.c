#include <zephyr/devicetree.h>
#include "zones.h"

#define DT_DRV_COMPAT acme_thermostat

#define ZONE_ENTRY(inst)                                                                           \
	{                                                                                          \
		.name = DT_INST_PROP(inst, zone_name),                                             \
		.setpoint = DT_INST_PROP(inst, setpoint_mdeg),                                     \
		.hysteresis = DT_INST_PROP(inst, hysteresis_mdeg),                                 \
		.cooling = DT_INST_ENUM_IDX(inst, mode) == 1,                                      \
	},

const struct zone zones[] = {DT_INST_FOREACH_STATUS_OKAY(ZONE_ENTRY)};
const int zone_count = DT_NUM_INST_STATUS_OKAY(DT_DRV_COMPAT);

bool zone_control(const struct zone *z, int32_t mdeg, bool on)
{
	int32_t lo = z->setpoint - z->hysteresis / 2, hi = z->setpoint + z->hysteresis / 2;

	if (z->cooling) {
		return mdeg > hi ? true : (mdeg < lo ? false : on);
	}
	return mdeg < lo ? true : (mdeg > hi ? false : on);
}
