#include <zephyr/ztest.h>
#include "filter.h"

static struct ema e;

static void before(void *f)
{
	ARG_UNUSED(f);
	ema_init(&e, 2);
}

ZTEST_SUITE(filter, NULL, NULL, before, NULL, NULL);

ZTEST(filter, test_first_sample_primes)
{
	zassert_equal(ema_add(&e, 100), 100);
	zassert_true(e.primed);
}

ZTEST(filter, test_moves_a_quarter)
{
	ema_add(&e, 100);
	zassert_equal(ema_add(&e, 200), 125, "100 + (200-100)/4");
}

ZTEST(filter, test_min_max)
{
	const int32_t s[] = {50, -20, 300, 10};

	for (size_t i = 0; i < ARRAY_SIZE(s); i++) {
		ema_add(&e, s[i]);
	}
	zassert_equal(e.min, -20);
	zassert_equal(e.max, 300);
}
