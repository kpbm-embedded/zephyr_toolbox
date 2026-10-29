#include <zephyr/kernel.h>
#include "canio.h"

#define NODE 1

static struct canio node;

static void beat(struct k_work *w)
{
	ARG_UNUSED(w);
	canio_heartbeat(&node);
}
static K_WORK_DEFINE(beat_work, beat);
static void beat_timer(struct k_timer *t)
{
	ARG_UNUSED(t);
	k_work_submit(&beat_work); /* can_send may block: not from the timer ISR */
}
static K_TIMER_DEFINE(beat_tmr, beat_timer, NULL);

int main(void)
{
	int err = canio_init(&node, DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus)), NODE, CAN_MODE_NORMAL);

	printk("__name__: node %d on %s: %d\n", NODE, node.dev->name, err);
	if (err) {
		return 0;
	}
	k_timer_start(&beat_tmr, K_SECONDS(1), K_SECONDS(1));
	while (1) {
		if (canio_poll(&node, K_FOREVER) > 0) {
			printk("state %u, %u commands\n", node.state, node.commands);
		}
	}
	return 0;
}
