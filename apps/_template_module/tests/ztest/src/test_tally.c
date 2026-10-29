#include <zephyr/ztest.h>
#include <tally/tally.h>

static const struct device *a = DEVICE_DT_GET(DT_NODELABEL(tally_a));
static const struct device *b = DEVICE_DT_GET(DT_NODELABEL(tally_b));

static int fired;
static uint32_t fired_at;
static void on_threshold(const struct device *dev, uint32_t count, void *user)
{
	ARG_UNUSED(dev);
	*(int *)user += 1;
	fired_at = count;
}

static void before(void *f)
{
	ARG_UNUSED(f);
	tally_reset(a);
	tally_reset(b);
	fired = 0;
	tally_set_callback(a, on_threshold, &fired);
}

ZTEST_SUITE(tally, NULL, NULL, before, NULL, NULL);

ZTEST(tally, test_kconfig_enabled_by_devicetree)
{
	zassert_true(IS_ENABLED(CONFIG_TALLY));
	zassert_true(device_is_ready(a) && device_is_ready(b));
}

ZTEST(tally, test_initial_from_devicetree)
{
	zassert_equal(tally_get(a), 0, "binding default");
	zassert_equal(tally_get(b), 100);
}

ZTEST(tally, test_threshold_fires_once)
{
	zassert_ok(tally_add(a, 4));
	zassert_ok(tally_add(a, 7)); /* 11: crosses 10 */
	zassert_ok(tally_add(a, 5));
	zassert_equal(fired, 1);
	zassert_equal(fired_at, 11);
}

ZTEST(tally, test_overflow_rejected)
{
	zassert_ok(tally_add(b, 1));
	zassert_equal(tally_add(b, UINT32_MAX), -EOVERFLOW);
	zassert_equal(tally_get(b), 101);
}
