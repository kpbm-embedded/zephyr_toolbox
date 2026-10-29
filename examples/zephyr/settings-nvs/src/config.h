#ifndef CONFIG_H_
#define CONFIG_H_
#include <stdint.h>

/* the app's persistent configuration: "app/boot_count", "app/name", "app/gain" */
struct app_config {
	uint32_t boot_count;
	char name[16];
	int16_t gain;
};

extern struct app_config app_config;
extern int app_config_commits;

void app_config_defaults(void);
int app_config_save(void);

#endif
