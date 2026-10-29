#include <zephyr/ztest.h>
#include <zephyr/drivers/can.h>
#include "proto.h"

static const struct device *can = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));
CAN_MSGQ_DEFINE(rx_q, 8);
CAN_MSGQ_DEFINE(ext_q, 2);
static int status_filter;

static void *setup(void)
{
	zassert_true(device_is_ready(can));
	/* loopback mode: sent frames go to our own filters (a real controller does the same in this mode) */
	zassert_ok(can_set_mode(can, CAN_MODE_LOOPBACK));
	zassert_ok(can_start(can));
	status_filter = can_add_rx_filter_msgq(can, &rx_q, &proto_status_filter);
	zassert_true(status_filter >= 0, "no free filter: %d", status_filter);
	return NULL;
}

static void before(void *f)
{
	ARG_UNUSED(f);
	k_msgq_purge(&rx_q);
}

ZTEST_SUITE(can_loopback, NULL, setup, before, NULL, NULL);

ZTEST(can_loopback, test_status_round_trip)
{
	struct node_status out = {.node = 5, .state = 2, .errors = 1, .mv = 12345}, in;
	struct can_frame f;

	proto_encode(&out, &f);
	zassert_equal(f.id, 0x105);
	zassert_ok(can_send(can, &f, K_MSEC(100), NULL, NULL));
	zassert_ok(k_msgq_get(&rx_q, &f, K_MSEC(100)), "frame not looped back");
	zassert_ok(proto_decode(&f, &in));
	zassert_equal(in.node, 5);
	zassert_equal(in.mv, 12345);
}

ZTEST(can_loopback, test_mask_filter_drops_other_ids)
{
	struct can_frame f = {.id = 0x200, .dlc = 1};

	zassert_ok(can_send(can, &f, K_MSEC(100), NULL, NULL));
	zassert_equal(k_msgq_get(&rx_q, &f, K_MSEC(50)), -EAGAIN, "0x200 must not pass the 0x10x filter");
}

ZTEST(can_loopback, test_extended_id_needs_its_own_filter)
{
	const struct can_filter ext = {.id = 0x18ff50e5, .mask = CAN_EXT_ID_MASK, .flags = CAN_FILTER_IDE};
	struct can_frame f = {.id = 0x18ff50e5, .flags = CAN_FRAME_IDE, .dlc = 8};
	int id = can_add_rx_filter_msgq(can, &ext_q, &ext);

	zassert_true(id >= 0);
	zassert_ok(can_send(can, &f, K_MSEC(100), NULL, NULL));
	zassert_ok(k_msgq_get(&ext_q, &f, K_MSEC(100)));
	zassert_true(f.flags & CAN_FRAME_IDE);
	zassert_equal(k_msgq_num_used_get(&rx_q), 0, "the standard-ID filter ignores extended frames");
	can_remove_rx_filter(can, id);
}

static K_SEM_DEFINE(tx_done, 0, 1);
static int tx_err = 1;
static void tx_cb(const struct device *dev, int error, void *user)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(user);
	tx_err = error;
	k_sem_give(&tx_done);
}

ZTEST(can_loopback, test_async_send_callback)
{
	struct can_frame f = {.id = 0x101, .dlc = 4};

	zassert_ok(can_send(can, &f, K_MSEC(100), tx_cb, NULL));
	zassert_ok(k_sem_take(&tx_done, K_MSEC(100)));
	zassert_equal(tx_err, 0);
	/* the callback can come before the looped-back frame reaches the filters: wait for it too */
	zassert_ok(k_msgq_get(&rx_q, &f, K_MSEC(100)));
	zassert_equal(f.id, 0x101);
}

ZTEST(can_loopback, test_decode_rejects_foreign_frames)
{
	struct node_status s;
	struct can_frame f = {.id = 0x105, .dlc = 2};

	zassert_equal(proto_decode(&f, &s), -EINVAL, "wrong length");
	f = (struct can_frame){.id = 0x105, .flags = CAN_FRAME_IDE, .dlc = 4};
	zassert_equal(proto_decode(&f, &s), -EINVAL, "extended id");
}

ZTEST(can_loopback, test_state_is_error_active)
{
	enum can_state state;
	struct can_bus_err_cnt cnt;

	zassert_ok(can_get_state(can, &state, &cnt));
	zassert_equal(state, CAN_STATE_ERROR_ACTIVE);
	zassert_equal(cnt.tx_err_cnt, 0);
}
