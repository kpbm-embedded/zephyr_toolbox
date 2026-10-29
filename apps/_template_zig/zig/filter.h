#ifndef FILTER_H
#define FILTER_H
#include <stdbool.h>
#include <stdint.h>

/* same layout as `extern struct Ema` in filter.zig */
struct ema {
	int32_t value;
	int32_t min;
	int32_t max;
	uint8_t shift;
	bool primed;
};

void ema_init(struct ema *e, uint8_t shift);
int32_t ema_add(struct ema *e, int32_t sample);

#endif
