#include <string.h>
#include <zephyr/ztest.h>
#include <zephyr/devicetree.h>
#include "zones.h"

ZTEST_SUITE(dt_kconfig, NULL, NULL, NULL, NULL, NULL);

static const struct zone *find(const char *name)
{
	for (int i = 0; i < zone_count; i++) {
		if (strcmp(zones[i].name, name) == 0) {
			return &zones[i];
		}
	}
	return NULL;
}

ZTEST(dt_kconfig, test_node_properties)
{
	/* the same values, straight from devicetree macros (all resolved at compile time) */
	BUILD_ASSERT(DT_PROP(DT_NODELABEL(zone_living), setpoint_mdeg) == 21000);
	BUILD_ASSERT(DT_PROP(DT_NODELABEL(zone_living), hysteresis_mdeg) == 500, "binding default");
	BUILD_ASSERT(DT_ENUM_IDX(DT_NODELABEL(zone_cellar), mode) == 1);
	zassert_true(DT_NODE_HAS_STATUS_OKAY(DT_NODELABEL(zone_living)));
	zassert_false(DT_NODE_HAS_STATUS_OKAY(DT_NODELABEL(zone_spare)));
}

ZTEST(dt_kconfig, test_table_from_devicetree)
{
	zassert_equal(zone_count, CONFIG_APP_EXPECT_ZONES);
	zassert_not_null(find("living"));
	zassert_not_null(find("cellar"));
	zassert_is_null(find("spare"), "disabled node must not be in the table");
	zassert_equal(find("cellar")->hysteresis, 1000);
	zassert_true(find("cellar")->cooling);
}

ZTEST(dt_kconfig, test_kconfig_values)
{
	zassert_true(IS_ENABLED(CONFIG_APP_VERBOSE), "set in prj.conf");
	zassert_true(CONFIG_APP_SAMPLE_PERIOD_MS == 1000 || CONFIG_APP_SAMPLE_PERIOD_MS == 250,
		     "1000 by default, 250 in the 'fast' twister variant");
}

ZTEST(dt_kconfig, test_kconfig_from_devicetree)
{
	/* APP_ZONE_SERVER follows $(dt_nodelabel_enabled,zone_server) */
	zassert_equal(IS_ENABLED(CONFIG_APP_ZONE_SERVER), find("server") != NULL);
}

ZTEST(dt_kconfig, test_heating_hysteresis)
{
	const struct zone *z = find("living"); /* 21.0 C +- 0.25 */
	bool on = false;

	on = zone_control(z, 20000, on);
	zassert_true(on);
	on = zone_control(z, 21100, on); /* inside the band: keep state */
	zassert_true(on);
	on = zone_control(z, 21300, on);
	zassert_false(on);
}

ZTEST(dt_kconfig, test_cooling_hysteresis)
{
	const struct zone *z = find("cellar"); /* 14.0 C +- 0.5, cooling */

	zassert_true(zone_control(z, 15000, false));
	zassert_true(zone_control(z, 14200, true));
	zassert_false(zone_control(z, 13400, true));
}
