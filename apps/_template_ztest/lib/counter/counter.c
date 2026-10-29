#include "counter.h"

void counter_init(struct counter *c, uint32_t modulo)
{
	c->value = 0;
	c->wraps = 0;
	c->modulo = modulo ? modulo : 1;
}

void counter_tick(struct counter *c)
{
	if (++c->value == c->modulo) {
		c->value = 0;
		c->wraps++;
	}
}
