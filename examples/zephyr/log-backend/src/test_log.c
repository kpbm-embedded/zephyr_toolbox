#include <string.h>
#include <zephyr/ztest.h>
#include <zephyr/logging/log_ctrl.h>
#include "capture_backend.h"
#include "sensor.h"

static int16_t sensor_src;

static void *setup(void)
{
	sensor_src = (int16_t)log_source_id_get("sensor");
	zassert_true(sensor_src >= 0, "log source 'sensor' not found");
	return NULL;
}

static void before(void *f)
{
	ARG_UNUSED(f);
	log_backend_enable(capture_backend, NULL, LOG_LEVEL_DBG);
	log_filter_set(capture_backend, Z_LOG_LOCAL_DOMAIN_ID, sensor_src, LOG_LEVEL_DBG);
	capture_clear();
}

ZTEST_SUITE(log_backend, NULL, setup, before, NULL, NULL);

ZTEST(log_backend, test_levels_and_tags)
{
	zassert_equal(sensor_check(50, 100), 0);
	zassert_equal(sensor_check(95, 100), 0);
	zassert_equal(sensor_check(120, 100), -1);
	zassert_not_null(strstr(capture_text(), "<inf> sensor: ok 50"), "got: %s", capture_text());
	zassert_not_null(strstr(capture_text(), "<wrn> sensor: near limit 95"));
	zassert_not_null(strstr(capture_text(), "<err> sensor: over range 120 > 100"));
	zassert_equal(capture_lines(), 3);
}

ZTEST(log_backend, test_debug_is_compiled_out)
{
	sensor_check(1, 100);
	/* the module's compile-time level is INF: the runtime filter cannot bring LOG_DBG back */
	zassert_is_null(strstr(capture_text(), "value 1"));
}

ZTEST(log_backend, test_runtime_filter_per_backend)
{
	log_filter_set(capture_backend, Z_LOG_LOCAL_DOMAIN_ID, sensor_src, LOG_LEVEL_ERR);
	sensor_check(95, 100);  /* WRN: dropped for this backend only */
	sensor_check(150, 100); /* ERR: kept */
	zassert_is_null(strstr(capture_text(), "near limit"));
	zassert_not_null(strstr(capture_text(), "over range 150"));
	zassert_equal(capture_lines(), 1);
}

ZTEST(log_backend, test_hexdump)
{
	const uint8_t frame[] = {0xde, 0xad, 0xbe, 0xef};

	sensor_dump(frame, sizeof(frame));
	zassert_not_null(strstr(capture_text(), "frame"));
	zassert_not_null(strstr(capture_text(), "de ad be ef"), "got: %s", capture_text());
}

ZTEST(log_backend, test_backend_disable)
{
	log_backend_disable(capture_backend);
	sensor_check(150, 100);
	zassert_equal(capture_lines(), 0);
}
