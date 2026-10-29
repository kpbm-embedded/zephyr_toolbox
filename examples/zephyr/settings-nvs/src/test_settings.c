#include <string.h>
#include <zephyr/ztest.h>
#include <zephyr/settings/settings.h>
#include <zephyr/storage/flash_map.h>
#include "config.h"

static void *setup(void)
{
	const struct flash_area *fa;

	/* start from an erased partition, then mount the NVS backend */
	zassert_ok(flash_area_open(PARTITION_ID(storage_partition), &fa));
	zassert_ok(flash_area_erase(fa, 0, fa->fa_size));
	flash_area_close(fa);
	zassert_ok(settings_subsys_init());
	return NULL;
}

static void before(void *f)
{
	ARG_UNUSED(f);
	app_config_defaults();
	app_config_commits = 0;
}

ZTEST_SUITE(settings_nvs, NULL, setup, before, NULL, NULL);

ZTEST(settings_nvs, test_save_one_and_load)
{
	uint32_t count = 41;

	zassert_ok(settings_save_one("app/boot_count", &count, sizeof(count)));
	app_config_defaults();
	zassert_ok(settings_load());
	zassert_equal(app_config.boot_count, 41);
	zassert_true(app_config_commits >= 1, "commit() not called");
}

ZTEST(settings_nvs, test_export_all_then_reload)
{
	app_config.boot_count = 7;
	strcpy(app_config.name, "rov-1");
	app_config.gain = -3;
	zassert_ok(app_config_save());

	app_config_defaults(); /* as after a reset */
	zassert_ok(settings_load_subtree("app"));
	zassert_equal(app_config.boot_count, 7);
	zassert_str_equal(app_config.name, "rov-1");
	zassert_equal(app_config.gain, -3);
}

ZTEST(settings_nvs, test_newest_value_wins)
{
	for (uint32_t v = 100; v < 110; v++) {
		zassert_ok(settings_save_one("app/boot_count", &v, sizeof(v)));
	}
	app_config_defaults();
	zassert_ok(settings_load());
	zassert_equal(app_config.boot_count, 109);
}

ZTEST(settings_nvs, test_delete)
{
	const char name[] = "temp";

	zassert_ok(settings_save_one("app/name", name, sizeof(name)));
	zassert_ok(settings_delete("app/name"));
	app_config_defaults();
	zassert_ok(settings_load());
	zassert_str_equal(app_config.name, "unnamed", "deleted key must not come back");
}

ZTEST(settings_nvs, test_runtime_get_set)
{
	int16_t gain = 12, back = 0;

	/* runtime API: calls the handler's set()/get() directly, nothing is written to flash */
	zassert_ok(settings_runtime_set("app/gain", &gain, sizeof(gain)));
	zassert_equal(app_config.gain, 12);
	zassert_equal(settings_runtime_get("app/gain", &back, sizeof(back)), sizeof(back));
	zassert_equal(back, 12);
}

ZTEST(settings_nvs, test_many_writes_force_garbage_collection)
{
	/* 16 KiB / 1 KiB sectors: ~600 records of 24 bytes fill several sectors, NVS must recycle them */
	for (uint32_t v = 0; v < 600; v++) {
		zassert_ok(settings_save_one("app/boot_count", &v, sizeof(v)), "write %u", v);
	}
	app_config_defaults();
	zassert_ok(settings_load());
	zassert_equal(app_config.boot_count, 599);
}
