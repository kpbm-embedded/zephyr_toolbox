#ifndef CFG_H
#define CFG_H
#include <stdint.h>

struct cfg {
	uint32_t boots;
	char name[16];
};
extern struct cfg cfg;

void cfg_defaults(void);
int cfg_load(void);  /* settings_subsys_init() + load of "cfg/" (missing keys keep defaults) */
int cfg_save(void);  /* writes every key of "cfg/" */

#endif
