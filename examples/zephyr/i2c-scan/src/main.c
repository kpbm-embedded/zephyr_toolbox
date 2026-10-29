#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>

static const struct device *const bus = DEVICE_DT_GET(DT_PHANDLE(DT_PATH(zephyr_user), i2c_bus));

int main(void)
{
	if (!device_is_ready(bus)) {
		printk("I2C bus not ready\n");
		return 0;
	}
	while (1) {
		int found = 0;
		printk("scanning %s\n", bus->name);
		for (uint8_t a = 0x08; a < 0x78; a++) {
			uint8_t dummy;
			struct i2c_msg m = { .buf = &dummy, .len = 0, .flags = I2C_MSG_WRITE | I2C_MSG_STOP };
			if (i2c_transfer(bus, &m, 1, a) == 0) {
				printk("  device at 0x%02x\n", a);
				found++;
			}
		}
		printk("%d device(s)\n", found);
		k_sleep(K_SECONDS(5));
	}
}
