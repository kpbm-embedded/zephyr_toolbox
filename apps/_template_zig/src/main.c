#include <zephyr/kernel.h>
#include "filter.h"

int main(void)
{
	struct ema e;
	const int32_t samples[] = {100, 120, 80, 200, 100};

	ema_init(&e, 2);
	for (size_t i = 0; i < ARRAY_SIZE(samples); i++) {
		printk("sample %d -> ema %d\n", samples[i], ema_add(&e, samples[i]));
	}
	printk("__name__: min %d max %d\n", e.min, e.max);
	return 0;
}
