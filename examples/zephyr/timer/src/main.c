#include <zephyr/kernel.h>

static void expiry(struct k_timer *t)
{
	static unsigned int n;
	printk("periodic timer expiry %u at %lld ms\n", ++n, k_uptime_get());
}
static void stopped(struct k_timer *t) { printk("timer stopped\n"); }
K_TIMER_DEFINE(tmr, expiry, stopped);

int main(void)
{
	k_timer_start(&tmr, K_MSEC(500), K_MSEC(500));
	k_sleep(K_SECONDS(5));
	k_timer_stop(&tmr);
	printk("one-shot in 1 s\n");
	k_timer_start(&tmr, K_SECONDS(1), K_NO_WAIT);
	k_sleep(K_FOREVER);
}
