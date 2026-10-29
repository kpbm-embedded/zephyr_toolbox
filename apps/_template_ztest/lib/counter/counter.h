/* A modulo counter: the module the tests in tests/ztest cover. Replace with your own code. */
#ifndef COUNTER_H
#define COUNTER_H
#include <stdint.h>

struct counter {
	uint32_t value, modulo, wraps;
};

void counter_init(struct counter *c, uint32_t modulo);  /* modulo 0 is treated as 1 */
void counter_tick(struct counter *c);
#endif
