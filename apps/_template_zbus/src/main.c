#include <zephyr/kernel.h>
#include "bus.h"

int main(void)
{
	struct cmd limit = {.op = CMD_SET_LIMIT, .arg = 3};

	zbus_chan_pub(&cmd_chan, &limit, K_MSEC(100));
	for (uint32_t i = 0; i < 6; i++) {
		struct reading r = {.value = (int32_t)i, .seq = i};

		zbus_chan_pub(&reading_chan, &r, K_MSEC(100));
		printk("reading %u: last %d, over limit %u\n", i, bus_state.last.value, bus_state.over_limit);
		k_msleep(500);
	}
	return 0;
}
