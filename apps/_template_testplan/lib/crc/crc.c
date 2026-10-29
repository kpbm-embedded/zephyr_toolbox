#include <zephyr/sys/__assert.h>
#include "crc.h"

uint16_t crc16_ccitt(const uint8_t *data, size_t len)
{
	uint16_t crc = 0xffff;

	__ASSERT(data != NULL || len == 0, "NULL data");
	for (size_t i = 0; i < len; i++) {
		crc ^= (uint16_t)data[i] << 8;
		for (int b = 0; b < 8; b++) {
			crc = crc & 0x8000 ? (uint16_t)(crc << 1) ^ 0x1021 : (uint16_t)(crc << 1);
		}
	}
	return crc;
}
