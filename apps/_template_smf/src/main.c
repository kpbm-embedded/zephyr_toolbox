#include <zephyr/kernel.h>
#include "fsm.h"

K_MSGQ_DEFINE(events, sizeof(enum fsm_event), 8, 4);

/* producers (ISRs, other threads) only post events; this thread owns the machine */
static void fsm_thread(void *a, void *b, void *c)
{
	struct fsm f;
	enum fsm_event ev;

	fsm_init(&f);
	while (k_msgq_get(&events, &ev, K_FOREVER) == 0) {
		fsm_step(&f, ev);
		printk("event %d -> %s\n", ev, fsm_name(fsm_current(&f)));
	}
}
K_THREAD_DEFINE(fsm_tid, 1024, fsm_thread, NULL, NULL, NULL, 5, 0, 0);

int main(void)
{
	const enum fsm_event demo[] = {EV_BOOTED, EV_START, EV_STOP, EV_START, EV_FAIL, EV_RESET};

	for (size_t i = 0; i < ARRAY_SIZE(demo); i++) {
		k_msgq_put(&events, &demo[i], K_FOREVER);
		k_msleep(100);
	}
	return 0;
}
