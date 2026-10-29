#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>

static const struct adc_dt_spec ch = ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 0);

int main(void)
{
	int16_t buf;
	struct adc_sequence seq = { .buffer = &buf, .buffer_size = sizeof(buf) };

	if (!adc_is_ready_dt(&ch) || adc_channel_setup_dt(&ch) < 0 || adc_sequence_init_dt(&ch, &seq) < 0) {
		printk("ADC setup failed\n");
		return 0;
	}
	while (1) {
		int32_t mv;
		if (adc_read_dt(&ch, &seq) == 0) {
			mv = buf;
			if (adc_raw_to_millivolts_dt(&ch, &mv) < 0) { mv = -1; }
			printk("raw %d = %d mV\n", buf, mv);
		}
		k_msleep(500);
	}
}
