#ifndef CORE_H
#define CORE_H
#include <stdint.h>
#include <stddef.h>

/* processes a frame: logs DBG per frame, WRN on short frames, ERR on bad checksum; returns 0 if ok */
int core_process(const uint8_t *frame, size_t len);

#endif
