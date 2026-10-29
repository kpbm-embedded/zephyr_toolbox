#ifndef BUS_H
#define BUS_H
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

struct reading {
	int32_t value;
	uint32_t seq;
};

enum cmd_op { CMD_NONE, CMD_RESET, CMD_SET_LIMIT };
struct cmd {
	enum cmd_op op;
	int32_t arg;
};

ZBUS_CHAN_DECLARE(reading_chan, cmd_chan);

/* state kept by the observers in bus.c */
struct bus_state {
	struct reading last;   /* listener: latest valid reading */
	int32_t limit;         /* subscriber: changed by CMD_SET_LIMIT */
	uint32_t over_limit;   /* listener: readings above limit */
	uint32_t cmds;         /* subscriber: commands handled */
};
extern struct bus_state bus_state;
extern struct k_sem cmd_done;

#endif
