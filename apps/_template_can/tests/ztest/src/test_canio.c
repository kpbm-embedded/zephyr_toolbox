#include <zephyr/ztest.h>
#include <zephyr/sys/byteorder.h>
#include "canio.h"

static struct canio node;
CAN_MSGQ_DEFINE(hb_q, 4);

static void *setup(void)
{
	const struct device *dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));
	const struct can_filter hb = {.id = CANIO_HEARTBEAT_BASE + 3, .mask = CAN_STD_ID_MASK};

	/* loopback mode: the node receives its own frames, so one image plays both ends */
	zassert_ok(canio_init(&node, dev, 3, CAN_MODE_LOOPBACK));
	zassert_true(can_add_rx_filter_msgq(dev, &hb_q, &hb) >= 0);
	return NULL;
}

ZTEST_SUITE(canio, NULL, setup, NULL, NULL, NULL);

static void send_cmd(uint8_t op, uint8_t arg)
{
	struct can_frame f = {.id = CANIO_COMMAND_BASE + 3, .dlc = 2, .data = {op, arg}};

	zassert_ok(can_send(node.dev, &f, K_MSEC(100), NULL, NULL));
}

ZTEST(canio, test_heartbeat)
{
	struct can_frame f;

	node.state = 7;
	zassert_ok(canio_heartbeat(&node));
	zassert_ok(k_msgq_get(&hb_q, &f, K_MSEC(100)));
	zassert_equal(f.id, 0x703);
	zassert_equal(f.dlc, 5);
	zassert_equal(f.data[0], 7);
}

ZTEST(canio, test_set_state_command)
{
	send_cmd(OP_SET_STATE, 42);
	zassert_equal(canio_poll(&node, K_MSEC(100)), 1);
	zassert_equal(node.state, 42);
}

ZTEST(canio, test_other_node_ignored)
{
	struct can_frame f = {.id = CANIO_COMMAND_BASE + 4, .dlc = 2, .data = {OP_SET_STATE, 9}};
	uint32_t before = node.commands;

	zassert_ok(can_send(node.dev, &f, K_MSEC(100), NULL, NULL));
	zassert_equal(canio_poll(&node, K_MSEC(50)), 0);
	zassert_equal(node.commands, before);
}
