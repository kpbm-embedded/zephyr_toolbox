#include <string.h>
#include <zephyr/ztest.h>
#include <zephyr/logging/log_backend.h>
#include <zephyr/logging/log_ctrl.h>
#include "core.h"

/* a minimal backend that counts messages per level (immediate mode: counted inside the LOG_* call) */
static int count[5];
static void process(const struct log_backend *const b, union log_msg_generic *msg)
{
	ARG_UNUSED(b);
	uint8_t level = log_msg_get_level(&msg->log);

	if (level < ARRAY_SIZE(count)) {
		count[level]++;
	}
}
/* panic() is required: the logger calls it on a fatal error and when ztest finishes */
static void panic(const struct log_backend *const b)
{
	ARG_UNUSED(b);
}
static const struct log_backend_api api = {.process = process, .panic = panic};
LOG_BACKEND_DEFINE(counter_backend, api, true);

static void before(void *f)
{
	ARG_UNUSED(f);
	memset(count, 0, sizeof(count));
}

ZTEST_SUITE(core, NULL, NULL, before, NULL, NULL);

ZTEST(core, test_good_frame_logs_info)
{
	const uint8_t f[] = {1, 2, 3, 6};

	zassert_ok(core_process(f, sizeof(f)));
	zassert_equal(count[LOG_LEVEL_INF], 1);
	zassert_equal(count[LOG_LEVEL_ERR], 0);
}

ZTEST(core, test_errors_and_warnings)
{
	const uint8_t f[] = {1, 2, 3, 7};

	zassert_equal(core_process(f, sizeof(f)), -EBADMSG);
	zassert_equal(core_process(f, 1), -EINVAL);
	zassert_equal(count[LOG_LEVEL_ERR], 1);
	zassert_equal(count[LOG_LEVEL_WRN], 1);
}

ZTEST(core, test_debug_follows_kconfig_level)
{
	const uint8_t f[] = {1, 2, 3, 7};
	int expect = IS_ENABLED(CONFIG_APP_CORE_LOG_LEVEL_DBG) ? 2 : 0; /* "frame of", hexdump */

	core_process(f, sizeof(f));
	zassert_equal(count[LOG_LEVEL_DBG], expect, "CONFIG_APP_CORE_LOG_LEVEL=%d", CONFIG_APP_CORE_LOG_LEVEL);
}
