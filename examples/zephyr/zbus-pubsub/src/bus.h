#ifndef BUS_H
#define BUS_H

#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

/* one sample of a temperature sensor, published on temp_chan */
struct temp_msg {
	int32_t mdeg; /* milli-degrees Celsius, valid -40000..150000 */
	uint32_t seq;
};

ZBUS_CHAN_DECLARE(temp_chan);
ZBUS_OBS_DECLARE(temp_lis);

/* what each kind of observer has seen (reset by bus_reset_stats) */
struct bus_stats {
	atomic_t listener_calls;  /* synchronous, in the publisher's context */
	atomic_t subscriber_wakes; /* thread, notified; reads the channel (may skip values) */
	atomic_t msub_msgs;       /* thread, gets a copy of every message */
	struct temp_msg listener_last;
	struct temp_msg subscriber_last;
	struct temp_msg msub_seen[8];
};

extern struct bus_stats bus_stats;
extern struct k_sem subscriber_sem;
extern struct k_sem msub_sem;

void bus_reset_stats(void);

#endif
