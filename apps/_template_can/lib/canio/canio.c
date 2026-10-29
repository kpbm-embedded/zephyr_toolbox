#include <zephyr/sys/byteorder.h>
#include "canio.h"

CAN_MSGQ_DEFINE(cmd_q, 8);

int canio_init(struct canio *c, const struct device *dev, uint8_t node, can_mode_t mode)
{
	const struct can_filter cmd = {.id = CANIO_COMMAND_BASE + node, .mask = CAN_STD_ID_MASK};
	int err;

	*c = (struct canio){.dev = dev, .node = node};
	if (!device_is_ready(dev)) {
		return -ENODEV;
	}
	err = can_set_mode(dev, mode);
	if (!err) {
		err = can_start(dev);
	}
	if (!err) {
		err = can_add_rx_filter_msgq(dev, &cmd_q, &cmd);
	}
	return err < 0 ? err : 0;
}

int canio_heartbeat(struct canio *c)
{
	struct can_frame f = {.id = CANIO_HEARTBEAT_BASE + c->node, .dlc = 5};

	f.data[0] = c->state;
	sys_put_le32(k_uptime_seconds() - c->uptime_base_s, &f.data[1]);
	return can_send(c->dev, &f, K_MSEC(100), NULL, NULL);
}

int canio_poll(struct canio *c, k_timeout_t timeout)
{
	struct can_frame f;
	int n = 0;

	while (k_msgq_get(&cmd_q, &f, n ? K_NO_WAIT : timeout) == 0) {
		if (f.dlc >= 2 && f.data[0] == OP_SET_STATE) {
			c->state = f.data[1];
		} else if (f.dlc >= 1 && f.data[0] == OP_RESET_UPTIME) {
			c->uptime_base_s = (uint32_t)k_uptime_seconds();
		}
		c->commands++;
		n++;
	}
	return n;
}
