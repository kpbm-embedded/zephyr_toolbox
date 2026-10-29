#include <zephyr/kernel.h>
#include "sampler.h"

static int32_t counter;
static int32_t read_counter(void)
{
	return counter++; /* replace with a sensor/ADC read */
}

int main(void)
{
	int32_t buf[16];

	sampler_start(50, read_counter);
	for (int round = 0; round < 5; round++) {
		k_msleep(200);
		size_t n = sampler_drain(buf, ARRAY_SIZE(buf));

		printk("round %d: %u samples, first %d, dropped %u\n", round, (unsigned int)n, n ? buf[0] : -1,
		       sampler_dropped());
	}
	sampler_stop();
	return 0;
}
