#ifndef PROTO_H
#define PROTO_H
#include <zephyr/drivers/can.h>

/* A tiny app protocol: node <n> sends its status on ID 0x100 + n (11-bit), 4 bytes:
 * [0] state, [1] error count, [2..3] voltage in mV, little endian.
 */
#define PROTO_STATUS_BASE 0x100

struct node_status {
	uint8_t node;
	uint8_t state;
	uint8_t errors;
	uint16_t mv;
};

void proto_encode(const struct node_status *s, struct can_frame *f);
int proto_decode(const struct can_frame *f, struct node_status *s); /* -EINVAL if not a status frame */
/* receive filter for the status frames of all 16 nodes (0x100..0x10f) */
extern const struct can_filter proto_status_filter;

#endif
