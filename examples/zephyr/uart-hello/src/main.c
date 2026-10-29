#include <stdio.h>
#include <zephyr/kernel.h>

int main(void)
{
	printk("hello from %s\n", CONFIG_BOARD_TARGET);
	for (unsigned int n = 0;; n++) {
		printf("tick %u uptime %lld ms\n", n, k_uptime_get());
		k_sleep(K_SECONDS(1));
	}
}
