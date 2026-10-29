#include <zephyr/ztest.h>
#include "bus.h"

static void before(void *f)
{
	struct cmd reset = {.op = CMD_RESET};

	ARG_UNUSED(f);
	zbus_chan_pub(&cmd_chan, &reset, K_MSEC(100));
	k_sem_take(&cmd_done, K_MSEC(100));
	k_sem_reset(&cmd_done);
}

ZTEST_SUITE(bus, NULL, NULL, before, NULL, NULL);

static void publish(int32_t v)
{
	struct reading r = {.value = v};

	zassert_ok(zbus_chan_pub(&reading_chan, &r, K_MSEC(100)));
}

ZTEST(bus, test_listener_keeps_last)
{
	publish(7);
	zassert_equal(bus_state.last.value, 7);
}

ZTEST(bus, test_limit_command)
{
	struct cmd c = {.op = CMD_SET_LIMIT, .arg = 10};

	zassert_ok(zbus_chan_pub(&cmd_chan, &c, K_MSEC(100)));
	zassert_ok(k_sem_take(&cmd_done, K_MSEC(200)));
	publish(5);
	publish(11);
	publish(12);
	zassert_equal(bus_state.over_limit, 2);
}

ZTEST(bus, test_invalid_reading_rejected)
{
	struct reading r = {.value = -5000};

	publish(1);
	zassert_equal(zbus_chan_pub(&reading_chan, &r, K_MSEC(100)), -ENOMSG);
	zassert_equal(bus_state.last.value, 1);
}
