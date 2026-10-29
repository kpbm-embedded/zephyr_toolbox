#ifndef KEYS_H
#define KEYS_H
#include <zephyr/kernel.h>

/* The app's view of the keys: a menu cursor that moves on ENTER (select) / BACK presses, fed by
 * input events from any device (the gpio-keys buttons or software events).
 */
struct key_log {
	uint16_t code;
	int32_t value;
	const struct device *dev;
};

extern struct k_msgq key_q;   /* every key event, in order */
extern atomic_t menu_depth;   /* +1 on ENTER press, -1 on BACK press, never below 0 */

#endif
