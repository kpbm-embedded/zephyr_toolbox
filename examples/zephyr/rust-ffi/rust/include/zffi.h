/* C side of rust/src/lib.rs: keep in sync by hand (or generate with cbindgen) */
#ifndef ZFFI_H
#define ZFFI_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct zffi_pi {
	float kp, ki, integral, out_min, out_max;
};

uint16_t zffi_crc16(const uint8_t *data, size_t len);
bool zffi_checked_add(uint32_t a, uint32_t b, uint32_t *out);
float zffi_pi_step(struct zffi_pi *pi, float error, float dt);

#endif
