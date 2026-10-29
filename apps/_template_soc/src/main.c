/*
 * Application-SoC template: console banner, uptime heartbeat, led0 blink when the board
 * has a led0 alias, and the Zephyr shell on the console UART (try: kernel uptime, app info).
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/shell/shell.h>

#if DT_NODE_HAS_STATUS_OKAY(DT_ALIAS(led0))
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
#define HAVE_LED 1
#else
#define HAVE_LED 0
#endif

static int cmd_info(const struct shell *sh, size_t argc, char **argv)
{
	shell_print(sh, "board %s, %u Hz cycle counter, uptime %lld ms", CONFIG_BOARD_TARGET,
		    sys_clock_hw_cycles_per_sec(), k_uptime_get());
	return 0;
}
SHELL_CMD_REGISTER(info, NULL, "board and timer info", cmd_info);

int main(void)
{
	printk("app on %s\n", CONFIG_BOARD_TARGET);
#if HAVE_LED
	if (!gpio_is_ready_dt(&led) || gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE) < 0) {
		printk("led0 not ready\n");
	}
#endif
	while (1) {
#if HAVE_LED
		gpio_pin_toggle_dt(&led);
#endif
		k_msleep(500);
	}
	return 0;
}
