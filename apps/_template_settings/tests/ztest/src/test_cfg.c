#include <string.h>
#include <zephyr/ztest.h>
#include <zephyr/storage/flash_map.h>
#include "cfg.h"

static void *setup(void)
{
	const struct flash_area *fa;

	zassert_ok(flash_area_open(PARTITION_ID(storage_partition), &fa));
	zassert_ok(flash_area_erase(fa, 0, fa->fa_size));
	flash_area_close(fa);
	return NULL;
}

ZTEST_SUITE(cfg, NULL, setup, NULL, NULL, NULL);

ZTEST(cfg, test_1_defaults_on_empty_flash)
{
	zassert_ok(cfg_load());
	zassert_equal(cfg.boots, 0);
	zassert_str_equal(cfg.name, "__name__");
}

ZTEST(cfg, test_2_round_trip)
{
	zassert_ok(cfg_load());
	cfg.boots = 5;
	strcpy(cfg.name, "unit-7");
	zassert_ok(cfg_save());
	zassert_ok(cfg_load()); /* resets to defaults, then reads flash */
	zassert_equal(cfg.boots, 5);
	zassert_str_equal(cfg.name, "unit-7");
}
