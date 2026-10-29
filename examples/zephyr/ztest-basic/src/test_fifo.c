/* ztest suite for fifo.c: a fixture (setup/before), zassert/zexpect, a skipped case and a second suite. */
#include <zephyr/ztest.h>
#include "fifo.h"

struct fifo_fixture {
	struct fifo f;
};

static void *fifo_setup(void)
{
	static struct fifo_fixture fx;   /* once per suite */
	return &fx;
}

static void fifo_before(void *data)  /* before every case: a fresh FIFO */
{
	struct fifo_fixture *fx = data;
	fifo_init(&fx->f);
}

ZTEST_SUITE(fifo, NULL, fifo_setup, fifo_before, NULL, NULL);

ZTEST_F(fifo, test_starts_empty)
{
	uint8_t v;

	zassert_equal(fifo_count(&fixture->f), 0);
	zassert_false(fifo_get(&fixture->f, &v), "get on an empty FIFO must fail");
}

ZTEST_F(fifo, test_order_is_kept)
{
	uint8_t v;

	for (uint8_t i = 1; i <= 3; i++) {
		zassert_true(fifo_put(&fixture->f, i));
	}
	for (uint8_t i = 1; i <= 3; i++) {
		zassert_true(fifo_get(&fixture->f, &v));
		zassert_equal(v, i, "got %u, want %u", v, i);
	}
}

ZTEST_F(fifo, test_full_rejects)
{
	for (int i = 0; i < FIFO_CAP; i++) {
		zassert_true(fifo_put(&fixture->f, (uint8_t)i));
	}
	zassert_false(fifo_put(&fixture->f, 0xff), "put on a full FIFO must fail");
	zassert_equal(fifo_count(&fixture->f), FIFO_CAP);
}

ZTEST_F(fifo, test_wraps_around)
{
	uint8_t v;

	/* zexpect_* records a failure and keeps going: every mismatch is reported, not only the first */
	for (int round = 0; round < 3 * FIFO_CAP; round++) {
		zexpect_true(fifo_put(&fixture->f, (uint8_t)round));
		zexpect_true(fifo_get(&fixture->f, &v));
		zexpect_equal(v, (uint8_t)round);
	}
	zassert_equal(fifo_count(&fixture->f), 0);
}

ZTEST(fifo, test_skipped_example)
{
	ztest_test_skip();   /* shows as SKIP in the report (a case that needs hardware, for example) */
}

/* a second suite without a fixture; its predicate decides whether it runs */
static bool always(const void *state)
{
	ARG_UNUSED(state);
	return true;
}

ZTEST_SUITE(fifo_limits, always, NULL, NULL, NULL, NULL);

ZTEST(fifo_limits, test_capacity_is_power_of_two)
{
	BUILD_ASSERT((FIFO_CAP & (FIFO_CAP - 1)) == 0, "FIFO_CAP must be a power of two");
	zassert_true(FIFO_CAP >= 2);
}
