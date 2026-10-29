#include <zephyr/ztest.h>
#include "fsm.h"

static struct fsm f;

static void before(void *x)
{
	ARG_UNUSED(x);
	fsm_init(&f);
}

ZTEST_SUITE(fsm, NULL, NULL, before, NULL, NULL);

ZTEST(fsm, test_boot_to_idle)
{
	zassert_equal(fsm_current(&f), ST_BOOT);
	fsm_step(&f, EV_START); /* ignored while booting */
	zassert_equal(fsm_current(&f), ST_BOOT);
	fsm_step(&f, EV_BOOTED);
	zassert_equal(fsm_current(&f), ST_IDLE);
}

ZTEST(fsm, test_start_stop_counts_runs)
{
	fsm_step(&f, EV_BOOTED);
	for (int i = 0; i < 3; i++) {
		fsm_step(&f, EV_START);
		zassert_equal(fsm_current(&f), ST_ACTIVE);
		fsm_step(&f, EV_STOP);
	}
	zassert_equal(f.active_runs, 3);
}

ZTEST(fsm, test_error_and_reset)
{
	fsm_step(&f, EV_BOOTED);
	fsm_step(&f, EV_START);
	fsm_step(&f, EV_FAIL);
	zassert_equal(fsm_current(&f), ST_ERROR);
	zassert_equal(f.last_error, -EIO);
	fsm_step(&f, EV_START);
	zassert_equal(fsm_current(&f), ST_ERROR, "only reset leaves ERROR");
	fsm_step(&f, EV_RESET);
	zassert_equal(fsm_current(&f), ST_IDLE);
	zassert_str_equal(fsm_name(fsm_current(&f)), "IDLE");
}
