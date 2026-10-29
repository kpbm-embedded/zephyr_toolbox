#ifndef DATALOG_H
#define DATALOG_H
#include <stdint.h>

#define DATALOG_DIR "/lfs"
#define DATALOG_MAX_RECORDS 64 /* per file; then log.bin becomes log.old */

struct record {
	uint32_t seq;
	uint32_t uptime_ms;
	int32_t value;
};

int datalog_append(int32_t value);       /* 0 or -errno */
int datalog_count(void);                 /* records in log.bin */
int datalog_read(int index, struct record *r); /* index into log.bin, 0 = oldest */
uint32_t datalog_rotations(void);

#endif
