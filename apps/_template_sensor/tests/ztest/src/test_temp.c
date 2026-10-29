#include <zephyr/ztest.h>
#include <zephyr/drivers/emul.h>
#include <zephyr/drivers/emul_sensor.h>
#include "temp.h"

#define TEMP_NODE DT_ALIAS(ambient_temp0)
static const struct device *dev = DEVICE_DT_GET(TEMP_NODE);
static const struct emul *emul = EMUL_DT_GET(TEMP_NODE);

static void set_mdeg(int32_t mdeg)
{
	q31_t q = (q31_t)(((int64_t)mdeg << (31 - 8)) / 1000); /* shift 8: +-256 C */

	zassert_ok(emul_sensor_backend_set_channel(emul, (struct sensor_chan_spec){SENSOR_CHAN_AMBIENT_TEMP, 0}, &q, 8));
}

ZTEST_SUITE(temp, NULL, NULL, NULL, NULL, NULL);

ZTEST(temp, test_read)
{
	int32_t mdeg;

	set_mdeg(36500);
	zassert_ok(temp_read_mdeg(dev, &mdeg));
	zassert_equal(mdeg, 36500);
}

ZTEST(temp, test_moving_average)
{
	struct temp_avg a = {0};
	const int32_t in[] = {20000, 22000, 24000, 26000, 28000};
	int32_t mdeg, avg = 0;

	for (size_t i = 0; i < ARRAY_SIZE(in); i++) {
		set_mdeg(in[i]);
		zassert_ok(temp_read_mdeg(dev, &mdeg));
		avg = temp_avg_add(&a, mdeg);
	}
	zassert_equal(avg, 25000, "average of the last 4: %d", avg); /* 22+24+26+28 */
}
