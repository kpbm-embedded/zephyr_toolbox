#ifndef CRC_H
#define CRC_H
#include <stddef.h>
#include <stdint.h>

/* CRC-16/CCITT-FALSE: poly 0x1021, init 0xffff, no reflection; "123456789" -> 0x29b1 */
uint16_t crc16_ccitt(const uint8_t *data, size_t len);

#endif
