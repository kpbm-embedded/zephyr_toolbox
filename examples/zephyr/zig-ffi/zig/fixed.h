/* C side of zig/fixed.zig (Zig can also emit it: zig build-obj -femit-h, not used here) */
#ifndef FIXED_H
#define FIXED_H
#include <stddef.h>
#include <stdint.h>

struct zf_avg {
	int32_t buf[4];
	uint32_t n;
	uint32_t pos;
};

int32_t zf_q16_mul(int32_t a, int32_t b);
int32_t zf_q16_from_milli(int32_t whole, int32_t milli);
uint16_t zf_fletcher16(const uint8_t *data, size_t len);
int32_t zf_avg_add(struct zf_avg *avg, int32_t v);

#endif
