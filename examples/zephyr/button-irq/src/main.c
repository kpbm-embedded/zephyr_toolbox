#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static struct gpio_callback cb;

static void pressed(const struct device *dev, struct gpio_callback *c, uint32_t pins)
{
	gpio_pin_toggle_dt(&led);
}

int main(void)
{
	if (!gpio_is_ready_dt(&led) || !gpio_is_ready_dt(&btn)) {
		return 0;
	}
	gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&btn, GPIO_INPUT);
	gpio_pin_interrupt_configure_dt(&btn, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&cb, pressed, BIT(btn.pin));
	gpio_add_callback(btn.port, &cb);
	printk("press the button (sw0) to toggle led0\n");
	k_sleep(K_FOREVER);
}
