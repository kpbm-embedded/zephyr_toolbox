#include <errno.h>
#include <string.h>
#include <zephyr/settings/settings.h>
#include "cfg.h"

struct cfg cfg;

void cfg_defaults(void)
{
	memset(&cfg, 0, sizeof(cfg));
	strcpy(cfg.name, "__name__");
}

static int set(const char *key, size_t len, settings_read_cb read_cb, void *cb_arg)
{
	if (strcmp(key, "boots") == 0 && len == sizeof(cfg.boots)) {
		return read_cb(cb_arg, &cfg.boots, len) < 0 ? -EIO : 0;
	}
	if (strcmp(key, "name") == 0 && len <= sizeof(cfg.name)) {
		return read_cb(cb_arg, cfg.name, len) < 0 ? -EIO : 0;
	}
	return -ENOENT;
}

SETTINGS_STATIC_HANDLER_DEFINE(cfg, "cfg", NULL, set, NULL, NULL);

int cfg_load(void)
{
	int err = settings_subsys_init();

	cfg_defaults();
	return err ? err : settings_load_subtree("cfg");
}

int cfg_save(void)
{
	int err = settings_save_one("cfg/boots", &cfg.boots, sizeof(cfg.boots));

	return err ? err : settings_save_one("cfg/name", cfg.name, strlen(cfg.name) + 1);
}
