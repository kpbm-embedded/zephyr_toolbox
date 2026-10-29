#ifndef CMDS_H
#define CMDS_H
#include <stdbool.h>

struct app_ctl {
	int gain;
	bool running;
};
extern struct app_ctl app_ctl;

#endif
