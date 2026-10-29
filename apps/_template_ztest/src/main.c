#include <zephyr/kernel.h>
#include "counter.h"

int main(void)
{
	struct counter c;

	counter_init(&c, 3);
	for (int i = 0; i < 5; i++) {
		counter_tick(&c);
		printk("tick %d: value %u wrapped %u\n", i, c.value, c.wraps);
		k_msleep(100);
	}
	return 0;
}
