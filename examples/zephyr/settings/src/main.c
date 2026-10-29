#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>

static uint32_t boot_count;

static int set_cb(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg)
{
	const char *next;
	if (settings_name_steq(name, "boots", &next) && !next) {
		return read_cb(cb_arg, &boot_count, sizeof(boot_count)) < 0 ? -EIO : 0;
	}
	return -ENOENT;
}
SETTINGS_STATIC_HANDLER_DEFINE(app, "app", NULL, set_cb, NULL, NULL);

int main(void)
{
	int rc = settings_subsys_init();
	if (rc) {
		printk("settings init failed (%d)\n", rc);
		return 0;
	}
	settings_load();
	boot_count++;
	rc = settings_save_one("app/boots", &boot_count, sizeof(boot_count));
	printk("boot count %u (save rc %d)\n", boot_count, rc);
	k_sleep(K_FOREVER);
}
