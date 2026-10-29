#include <zephyr/kernel.h>
#include "cfg.h"

int main(void)
{
	int err = cfg_load();

	if (err) {
		printk("settings load failed: %d\n", err);
		return 0;
	}
	cfg.boots++;
	err = cfg_save();
	printk("%s: boot %u (save %d)\n", cfg.name, cfg.boots, err);
	return 0;
}
