#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>

static const struct pwm_dt_spec led = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

int main(void)
{
	if (!pwm_is_ready_dt(&led)) {
		printk("PWM device not ready\n");
		return 0;
	}
	const uint32_t step = led.period / 50;
	uint32_t pulse = 0;
	int dir = 1;
	while (1) {
		pwm_set_pulse_dt(&led, pulse);
		if (dir > 0 && pulse + step >= led.period) { dir = -1; }
		else if (dir < 0 && pulse < step) { dir = 1; }
		pulse = dir > 0 ? pulse + step : pulse - step;
		k_msleep(20);
	}
}
