#include "bus.h"

struct bus_state bus_state = {.limit = 100};
K_SEM_DEFINE(cmd_done, 0, 8);

static bool reading_ok(const void *msg, size_t size)
{
	ARG_UNUSED(size);
	return ((const struct reading *)msg)->value > -1000;
}

static void on_reading(const struct zbus_channel *chan)
{
	const struct reading *r = zbus_chan_const_msg(chan);

	bus_state.last = *r;
	if (r->value > bus_state.limit) {
		bus_state.over_limit++;
	}
}

ZBUS_LISTENER_DEFINE(reading_lis, on_reading);
ZBUS_SUBSCRIBER_DEFINE(cmd_sub, 4);

ZBUS_CHAN_DEFINE(reading_chan, struct reading, reading_ok, NULL, ZBUS_OBSERVERS(reading_lis),
		 ZBUS_MSG_INIT(0));
ZBUS_CHAN_DEFINE(cmd_chan, struct cmd, NULL, NULL, ZBUS_OBSERVERS(cmd_sub), ZBUS_MSG_INIT(0));

static void cmd_thread(void *a, void *b, void *c)
{
	const struct zbus_channel *chan;
	struct cmd cmd;

	while (zbus_sub_wait(&cmd_sub, &chan, K_FOREVER) == 0) {
		if (zbus_chan_read(chan, &cmd, K_MSEC(100)) != 0) {
			continue;
		}
		if (cmd.op == CMD_RESET) {
			bus_state.over_limit = 0;
		} else if (cmd.op == CMD_SET_LIMIT) {
			bus_state.limit = cmd.arg;
		}
		bus_state.cmds++;
		k_sem_give(&cmd_done);
	}
}
K_THREAD_DEFINE(cmd_tid, 1024, cmd_thread, NULL, NULL, NULL, 5, 0, 0);
