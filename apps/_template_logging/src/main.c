#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "core.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
	const uint8_t good[] = {1, 2, 3, 6}, bad[] = {1, 2, 3, 7};

	LOG_INF("__name__ started");
	core_process(good, sizeof(good));
	core_process(bad, sizeof(bad));
	core_process(good, 1);
	return 0;
}
