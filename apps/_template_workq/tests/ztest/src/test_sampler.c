#include <zephyr/ztest.h>
#include "sampler.h"

static int32_t next;
static int32_t read_seq(void)
{
	return next++;
}

static void before(void *f)
{
	ARG_UNUSED(f);
	next = 0;
}

static void after(void *f)
{
	ARG_UNUSED(f);
	sampler_stop();
}

ZTEST_SUITE(sampler, NULL, NULL, before, after, NULL);

ZTEST(sampler, test_rate_and_order)
{
	int32_t buf[16];
	size_t n;

	sampler_start(10, read_seq);
	k_msleep(105);
	sampler_stop();
	n = sampler_drain(buf, ARRAY_SIZE(buf));
	zassert_within(n, 10, 1, "samples: %u", (unsigned int)n);
	for (size_t i = 0; i < n; i++) {
		zassert_equal(buf[i], (int32_t)i, "oldest first");
	}
}

ZTEST(sampler, test_stop_stops)
{
	int32_t buf[16];

	sampler_start(5, read_seq);
	k_msleep(20);
	sampler_stop();
	sampler_drain(buf, ARRAY_SIZE(buf));
	k_msleep(30);
	zassert_equal(sampler_drain(buf, ARRAY_SIZE(buf)), 0, "no samples after stop");
}

ZTEST(sampler, test_overflow_is_counted)
{
	int32_t buf[32];

	sampler_start(2, read_seq);
	k_msleep(100); /* 2 ms period + reschedule drift: 30 or more samples into a 16-slot buffer */
	sampler_stop();
	zassert_equal(sampler_drain(buf, ARRAY_SIZE(buf)), 16);
	zassert_true(sampler_dropped() >= 5, "dropped %u", sampler_dropped());
}
