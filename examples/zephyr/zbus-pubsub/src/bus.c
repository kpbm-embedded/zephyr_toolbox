/* Channel and observers: a listener (callback), a subscriber (thread, notification only) and a
 * message subscriber (thread, own copy of each message). The validator rejects impossible values.
 */
#include <string.h>
#include "bus.h"

struct bus_stats bus_stats;
K_SEM_DEFINE(subscriber_sem, 0, 16);
K_SEM_DEFINE(msub_sem, 0, 16);

static bool temp_valid(const void *msg, size_t msg_size)
{
	const struct temp_msg *m = msg;

	ARG_UNUSED(msg_size);
	return m->mdeg >= -40000 && m->mdeg <= 150000;
}

static void listener_cb(const struct zbus_channel *chan)
{
	const struct temp_msg *m = zbus_chan_const_msg(chan); /* channel is locked during the callback */

	bus_stats.listener_last = *m;
	atomic_inc(&bus_stats.listener_calls);
}

ZBUS_LISTENER_DEFINE(temp_lis, listener_cb);
ZBUS_SUBSCRIBER_DEFINE(temp_sub, 4);
ZBUS_MSG_SUBSCRIBER_DEFINE(temp_msub);

ZBUS_CHAN_DEFINE(temp_chan, struct temp_msg, temp_valid, NULL,
		 ZBUS_OBSERVERS(temp_lis, temp_sub, temp_msub), ZBUS_MSG_INIT(.mdeg = 0, .seq = 0));

static void subscriber_thread(void *a, void *b, void *c)
{
	const struct zbus_channel *chan;

	while (zbus_sub_wait(&temp_sub, &chan, K_FOREVER) == 0) {
		struct temp_msg m;

		if (zbus_chan_read(chan, &m, K_MSEC(100)) == 0) {
			bus_stats.subscriber_last = m;
			atomic_inc(&bus_stats.subscriber_wakes);
			k_sem_give(&subscriber_sem);
		}
	}
}

static void msub_thread(void *a, void *b, void *c)
{
	const struct zbus_channel *chan;
	struct temp_msg m;

	while (zbus_sub_wait_msg(&temp_msub, &chan, &m, K_FOREVER) == 0) {
		atomic_val_t n = atomic_inc(&bus_stats.msub_msgs);

		if (n < (atomic_val_t)ARRAY_SIZE(bus_stats.msub_seen)) {
			bus_stats.msub_seen[n] = m;
		}
		k_sem_give(&msub_sem);
	}
}

K_THREAD_DEFINE(sub_tid, 1024, subscriber_thread, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(msub_tid, 1024, msub_thread, NULL, NULL, NULL, 5, 0, 0);

void bus_reset_stats(void)
{
	memset(&bus_stats, 0, sizeof(bus_stats));
	k_sem_reset(&subscriber_sem);
	k_sem_reset(&msub_sem);
}
