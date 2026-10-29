/* Settings handler for the "app" subtree: set() restores values on settings_load(), export()
 * writes them all (settings_save), commit() runs once a load is complete.
 */
#include <errno.h>
#include <string.h>
#include <zephyr/settings/settings.h>
#include "config.h"

struct app_config app_config;
int app_config_commits;

void app_config_defaults(void)
{
	memset(&app_config, 0, sizeof(app_config));
	strcpy(app_config.name, "unnamed");
	app_config.gain = 1;
}

static int set(const char *key, size_t len, settings_read_cb read_cb, void *cb_arg)
{
	const char *next;
	void *dst = NULL;
	size_t size = 0;

	if (settings_name_steq(key, "boot_count", &next) && !next) {
		dst = &app_config.boot_count; size = sizeof(app_config.boot_count);
	} else if (settings_name_steq(key, "name", &next) && !next) {
		dst = app_config.name; size = sizeof(app_config.name);
	} else if (settings_name_steq(key, "gain", &next) && !next) {
		dst = &app_config.gain; size = sizeof(app_config.gain);
	} else {
		return -ENOENT;
	}
	if (len > size) {
		return -EINVAL;
	}
	memset(dst, 0, size);
	return read_cb(cb_arg, dst, len) >= 0 ? 0 : -EIO;
}

static int get(const char *key, char *val, int val_len_max)
{
	const char *next;

	if (settings_name_steq(key, "gain", &next) && !next && val_len_max >= (int)sizeof(app_config.gain)) {
		memcpy(val, &app_config.gain, sizeof(app_config.gain));
		return sizeof(app_config.gain);
	}
	return -ENOENT;
}

static int commit(void)
{
	app_config_commits++;
	return 0;
}

static int export(int (*cb)(const char *name, const void *value, size_t val_len))
{
	cb("app/boot_count", &app_config.boot_count, sizeof(app_config.boot_count));
	cb("app/name", app_config.name, strlen(app_config.name) + 1);
	return cb("app/gain", &app_config.gain, sizeof(app_config.gain));
}

SETTINGS_STATIC_HANDLER_DEFINE(app, "app", get, set, commit, export);

int app_config_save(void)
{
	return settings_save();
}
