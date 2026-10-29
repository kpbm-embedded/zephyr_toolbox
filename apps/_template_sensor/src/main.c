#include <zephyr/kernel.h>
#include "temp.h"

static const struct device *const sensor = DEVICE_DT_GET(DT_ALIAS(ambient_temp0));

int main(void)
{
	struct temp_avg avg = {0};
	int32_t mdeg;

	for (int i = 0; i < 5; i++) {
		int err = temp_read_mdeg(sensor, &mdeg);

		if (err) {
			printk("%s: read failed %d\n", sensor->name, err);
		} else {
			printk("%s: %d mC (avg %d)\n", sensor->name, mdeg, temp_avg_add(&avg, mdeg));
		}
		k_msleep(1000);
	}
	return 0;
}
