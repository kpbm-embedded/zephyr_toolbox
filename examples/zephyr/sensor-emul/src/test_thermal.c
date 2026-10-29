#include <zephyr/ztest.h>
#include <zephyr/drivers/emul.h>
#include <zephyr/drivers/emul_sensor.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor/f75303.h>
#include "thermal.h"

static const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(temp));
static const struct emul *emul = EMUL_DT_GET(DT_NODELABEL(temp));

/* set a channel of the emulated chip, in milli-degrees (q31 with shift 8: range +-256 C) */
static void emul_set_mdeg(enum sensor_channel ch, int32_t mdeg)
{
	q31_t q = (q31_t)(((int64_t)mdeg << (31 - 8)) / 1000);

	zassert_ok(emul_sensor_backend_set_channel(emul, (struct sensor_chan_spec){ch, 0}, &q, 8));
}

ZTEST_SUITE(sensor_emul, NULL, NULL, NULL, NULL, NULL);

ZTEST(sensor_emul, test_device_ready)
{
	zassert_true(device_is_ready(dev), "F75303 driver did not initialise over the emulated bus");
}

ZTEST(sensor_emul, test_read_ambient)
{
	struct sensor_value v;

	emul_set_mdeg(SENSOR_CHAN_AMBIENT_TEMP, 25500);
	zassert_ok(sensor_sample_fetch(dev));
	zassert_ok(sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &v));
	zassert_equal(v.val1, 25);
	zassert_equal(v.val2, 500000);
}

ZTEST(sensor_emul, test_remote_channels_and_resolution)
{
	struct sensor_value v;

	emul_set_mdeg((enum sensor_channel)SENSOR_CHAN_F75303_REMOTE1, 60130); /* 0.125 C steps: 60.125 */
	emul_set_mdeg((enum sensor_channel)SENSOR_CHAN_F75303_REMOTE2, 3000);
	zassert_ok(sensor_sample_fetch(dev));
	zassert_ok(sensor_channel_get(dev, (enum sensor_channel)SENSOR_CHAN_F75303_REMOTE1, &v));
	zassert_equal(v.val1 * 1000 + v.val2 / 1000, 60125);
	zassert_ok(sensor_channel_get(dev, (enum sensor_channel)SENSOR_CHAN_F75303_REMOTE2, &v));
	zassert_equal(v.val1, 3);
}

ZTEST(sensor_emul, test_sample_range)
{
	q31_t lower, upper, eps;
	int8_t shift;

	zassert_ok(emul_sensor_backend_get_sample_range(emul, (struct sensor_chan_spec){SENSOR_CHAN_AMBIENT_TEMP, 0},
							&lower, &upper, &eps, &shift));
	zassert_equal(shift, 8);
	/* epsilon = 0.125 C in q31 << 8 */
	zassert_equal(((int64_t)eps << shift) * 1000 / ((int64_t)INT32_MAX + 1), 125);
}

ZTEST(sensor_emul, test_thermal_states)
{
	int32_t mdeg;

	emul_set_mdeg(SENSOR_CHAN_AMBIENT_TEMP, 40000);
	zassert_equal(thermal_check(dev, 85000, &mdeg), THERMAL_OK);
	emul_set_mdeg(SENSOR_CHAN_AMBIENT_TEMP, 81000);
	zassert_equal(thermal_check(dev, 85000, &mdeg), THERMAL_WARN);
	emul_set_mdeg(SENSOR_CHAN_AMBIENT_TEMP, 90000);
	zassert_equal(thermal_check(dev, 85000, &mdeg), THERMAL_TRIP);
	zassert_equal(mdeg, 90000);
}
