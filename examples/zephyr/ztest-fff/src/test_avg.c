/* FFF (fake function framework, bundled with Zephyr) replaces sensor_hw_read() with a fake whose
 * return values and output argument the test scripts. */
#include <errno.h>
#include <zephyr/fff.h>
#include <zephyr/ztest.h>
#include "avg.h"
#include "sensor_hw.h"

DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC(int, sensor_hw_read, int16_t *);

static const int16_t samples[] = {10, 20, 30, 40};
static size_t next_sample;

static int read_from_table(int16_t *raw)   /* custom fake: hand out samples[] in order */
{
	*raw = samples[next_sample++ % ARRAY_SIZE(samples)];
	return 0;
}

static void reset(const struct ztest_unit_test *test, void *data)
{
	ARG_UNUSED(test);
	ARG_UNUSED(data);
	RESET_FAKE(sensor_hw_read);
	FFF_RESET_HISTORY();
	next_sample = 0;
}

ZTEST_RULE(reset_fakes, reset, NULL);   /* runs before every case of every suite */
ZTEST_SUITE(avg, NULL, NULL, NULL, NULL, NULL);

ZTEST(avg, test_average_of_four)
{
	int16_t out;

	sensor_hw_read_fake.custom_fake = read_from_table;
	zassert_ok(sensor_avg(4, &out));
	zassert_equal(out, 25);
	zassert_equal(sensor_hw_read_fake.call_count, 4);
}

ZTEST(avg, test_retries_after_error)
{
	int16_t out;
	int rets[] = {-EIO, 0, 0};

	SET_RETURN_SEQ(sensor_hw_read, rets, ARRAY_SIZE(rets));
	zassert_ok(sensor_avg(2, &out));
	zassert_equal(sensor_hw_read_fake.call_count, 3, "one retry expected");
}

ZTEST(avg, test_gives_up)
{
	int16_t out;

	sensor_hw_read_fake.return_val = -EIO;
	zassert_equal(sensor_avg(3, &out), -EIO);
	zassert_equal(sensor_hw_read_fake.call_count, 3);
}
