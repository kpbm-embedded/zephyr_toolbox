#include <errno.h>
#include <zephyr/logging/log.h>
#include "core.h"

/* level from Kconfig: CONFIG_APP_CORE_LOG_LEVEL (Kconfig of the app) */
LOG_MODULE_REGISTER(app_core, CONFIG_APP_CORE_LOG_LEVEL);

int core_process(const uint8_t *frame, size_t len)
{
	uint8_t sum = 0;

	LOG_DBG("frame of %zu bytes", len);
	if (len < 2) {
		LOG_WRN("short frame (%zu)", len);
		return -EINVAL;
	}
	for (size_t i = 0; i + 1 < len; i++) {
		sum += frame[i];
	}
	if (sum != frame[len - 1]) {
		LOG_ERR("checksum %02x != %02x", sum, frame[len - 1]);
		LOG_HEXDUMP_DBG(frame, len, "bad frame");
		return -EBADMSG;
	}
	LOG_INF("frame ok");
	return 0;
}
