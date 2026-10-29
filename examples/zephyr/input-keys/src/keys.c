#include <zephyr/input/input.h>
#include "keys.h"

K_MSGQ_DEFINE(key_q, sizeof(struct key_log), 16, 4);
atomic_t menu_depth;

/* NULL device: called for events of every input device */
static void on_input(struct input_event *evt, void *user_data)
{
	struct key_log k = {.code = evt->code, .value = evt->value, .dev = evt->dev};

	ARG_UNUSED(user_data);
	if (evt->type != INPUT_EV_KEY) {
		return;
	}
	if (evt->value == 1 && evt->code == INPUT_KEY_ENTER) {
		atomic_inc(&menu_depth);
	} else if (evt->value == 1 && evt->code == INPUT_KEY_BACK && atomic_get(&menu_depth) > 0) {
		atomic_dec(&menu_depth);
	}
	/* last: a reader woken by the queue sees menu_depth already updated */
	k_msgq_put(&key_q, &k, K_NO_WAIT);
}
INPUT_CALLBACK_DEFINE(NULL, on_input, NULL);
