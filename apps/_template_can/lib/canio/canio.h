#ifndef CANIO_H
#define CANIO_H
#include <zephyr/drivers/can.h>

#define CANIO_HEARTBEAT_BASE 0x700
#define CANIO_COMMAND_BASE 0x600

enum canio_op { OP_NOP, OP_SET_STATE, OP_RESET_UPTIME };

struct canio {
	const struct device *dev;
	uint8_t node;
	uint8_t state;
	uint32_t uptime_base_s;
	uint32_t commands;
};

int canio_init(struct canio *c, const struct device *dev, uint8_t node, can_mode_t mode);
int canio_heartbeat(struct canio *c);
/* handle queued commands; returns the number handled (waits up to timeout for the first one) */
int canio_poll(struct canio *c, k_timeout_t timeout);

#endif
