#include <zephyr/ztest.h>
#include "counter.h"

static struct counter c;

static void before(void *data)
{
	ARG_UNUSED(data);
	counter_init(&c, 3);
}

ZTEST_SUITE(counter, NULL, NULL, before, NULL, NULL);

ZTEST(counter, test_counts_up)
{
	counter_tick(&c);
	counter_tick(&c);
	zassert_equal(c.value, 2);
	zassert_equal(c.wraps, 0);
}

ZTEST(counter, test_wraps_at_modulo)
{
	for (int i = 0; i < 7; i++) {
		counter_tick(&c);
	}
	zassert_equal(c.value, 1);
	zassert_equal(c.wraps, 2);
}

ZTEST(counter, test_zero_modulo_is_one)
{
	counter_init(&c, 0);
	counter_tick(&c);
	zassert_equal(c.value, 0);
	zassert_equal(c.wraps, 1);
}
