#include <zephyr/ztest.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/input/input.h>
#include "keys.h"

static const struct device *keys_dev = DEVICE_DT_GET(DT_NODELABEL(buttons));
static const struct gpio_dt_spec ok = GPIO_DT_SPEC_GET(DT_NODELABEL(button_ok), gpios);
static const struct gpio_dt_spec back = GPIO_DT_SPEC_GET(DT_NODELABEL(button_back), gpios);

static void before(void *f)
{
	ARG_UNUSED(f);
	gpio_emul_input_set_dt(&ok, 0);
	gpio_emul_input_set_dt(&back, 0);
	k_msleep(30);
	k_msgq_purge(&key_q);
	atomic_clear(&menu_depth);
}

ZTEST_SUITE(input_keys, NULL, NULL, before, NULL, NULL);

static void click(const struct gpio_dt_spec *b)
{
	gpio_emul_input_set_dt(b, 1);
	k_msleep(30); /* > debounce-interval-ms */
	gpio_emul_input_set_dt(b, 0);
	k_msleep(30);
}

ZTEST(input_keys, test_driver_ready)
{
	zassert_true(device_is_ready(keys_dev));
}

ZTEST(input_keys, test_press_release_events)
{
	struct key_log k;

	click(&ok);
	zassert_ok(k_msgq_get(&key_q, &k, K_MSEC(100)));
	zassert_equal(k.code, INPUT_KEY_ENTER);
	zassert_equal(k.value, 1, "press");
	zassert_equal_ptr(k.dev, keys_dev);
	zassert_ok(k_msgq_get(&key_q, &k, K_MSEC(100)));
	zassert_equal(k.value, 0, "release");
}

ZTEST(input_keys, test_bounce_is_filtered)
{
	struct key_log k;

	for (int i = 0; i < 5; i++) { /* edges 1 ms apart: inside the 10 ms debounce window */
		gpio_emul_input_set_dt(&back, i & 1 ? 0 : 1);
		k_busy_wait(1000); /* not k_msleep(1): with 100 ticks/s that sleeps a whole 10 ms tick */
	}
	k_msleep(40); /* ends pressed (last write was 1) */
	zassert_ok(k_msgq_get(&key_q, &k, K_MSEC(100)));
	zassert_equal(k.code, INPUT_KEY_BACK);
	zassert_equal(k.value, 1);
	zassert_equal(k_msgq_num_used_get(&key_q), 0, "one event for a bouncing press");
}

ZTEST(input_keys, test_menu_logic)
{
	click(&ok);
	click(&ok);
	click(&back);
	zassert_equal(atomic_get(&menu_depth), 1);
	click(&back);
	click(&back); /* already at the top */
	zassert_equal(atomic_get(&menu_depth), 0);
}

ZTEST(input_keys, test_software_event)
{
	struct key_log k;

	/* any code can report events, e.g. a touch or a remote; dev NULL = no device */
	zassert_ok(input_report_key(NULL, INPUT_KEY_ENTER, 1, true, K_FOREVER));
	zassert_ok(k_msgq_get(&key_q, &k, K_MSEC(100)));
	zassert_is_null(k.dev);
	zassert_equal(atomic_get(&menu_depth), 1);
}
