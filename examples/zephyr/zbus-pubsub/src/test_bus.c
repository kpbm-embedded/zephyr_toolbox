#include <zephyr/ztest.h>
#include "bus.h"

static void before(void *f)
{
	ARG_UNUSED(f);
	k_msleep(20); /* let the observer threads drain what the previous case published */
	bus_reset_stats();
}

ZTEST_SUITE(zbus_pubsub, NULL, NULL, before, NULL, NULL);

static int publish(int32_t mdeg, uint32_t seq)
{
	struct temp_msg m = {.mdeg = mdeg, .seq = seq};

	return zbus_chan_pub(&temp_chan, &m, K_MSEC(100));
}

ZTEST(zbus_pubsub, test_listener_runs_synchronously)
{
	zassert_ok(publish(21500, 1));
	/* no wait: listeners run inside zbus_chan_pub */
	zassert_equal(atomic_get(&bus_stats.listener_calls), 1);
	zassert_equal(bus_stats.listener_last.mdeg, 21500);
}

ZTEST(zbus_pubsub, test_subscriber_is_notified)
{
	zassert_ok(publish(22000, 2));
	zassert_ok(k_sem_take(&subscriber_sem, K_MSEC(500)), "subscriber not woken");
	zassert_equal(bus_stats.subscriber_last.seq, 2);
}

ZTEST(zbus_pubsub, test_msg_subscriber_gets_every_message)
{
	for (uint32_t i = 1; i <= 3; i++) {
		zassert_ok(publish(20000 + (int32_t)i, i));
	}
	for (int i = 0; i < 3; i++) {
		zassert_ok(k_sem_take(&msub_sem, K_MSEC(500)), "message %d missing", i);
	}
	/* each message is a copy taken at publish time, so all three values arrive in order */
	for (int i = 0; i < 3; i++) {
		zassert_equal(bus_stats.msub_seen[i].seq, (uint32_t)i + 1);
	}
}

ZTEST(zbus_pubsub, test_validator_rejects_bad_value)
{
	struct temp_msg now;

	zassert_ok(publish(25000, 7));
	zassert_equal(publish(400000, 8), -ENOMSG, "400 C must be rejected");
	zassert_ok(zbus_chan_read(&temp_chan, &now, K_MSEC(100)));
	zassert_equal(now.seq, 7, "rejected message must not reach the channel");
	zassert_equal(atomic_get(&bus_stats.listener_calls), 1);
}

static atomic_t extra_calls;
static void extra_cb(const struct zbus_channel *chan)
{
	ARG_UNUSED(chan);
	atomic_inc(&extra_calls);
}
ZBUS_LISTENER_DEFINE(extra_lis, extra_cb);

ZTEST(zbus_pubsub, test_runtime_observer_add_remove)
{
	atomic_clear(&extra_calls);
	zassert_ok(zbus_chan_add_obs(&temp_chan, &extra_lis, K_MSEC(100)));
	zassert_ok(publish(23000, 9));
	zassert_ok(zbus_chan_rm_obs(&temp_chan, &extra_lis, K_MSEC(100)));
	zassert_ok(publish(23001, 10));
	zassert_equal(atomic_get(&extra_calls), 1, "removed observer must not be called");
}

ZTEST(zbus_pubsub, test_channel_and_observer_names)
{
	zassert_str_equal(zbus_chan_name(&temp_chan), "temp_chan");
	zassert_str_equal(zbus_obs_name(&temp_lis), "temp_lis");
}
