#include <zephyr/kernel.h>
#include <tally/tally.h>

static void on_threshold(const struct device *dev, uint32_t count, void *user)
{
	ARG_UNUSED(user);
	printk("%s reached %u\n", dev->name, count);
}

int main(void)
{
	const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(events));

	tally_set_callback(dev, on_threshold, NULL);
	for (int i = 0; i < 8; i++) {
		tally_add(dev, 1);
		k_msleep(100);
	}
	printk("count %u\n", tally_get(dev));
	return 0;
}
