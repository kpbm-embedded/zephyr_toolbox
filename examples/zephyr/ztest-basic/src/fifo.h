/* Fixed-size byte FIFO: the module under test. */
#ifndef FIFO_H
#define FIFO_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define FIFO_CAP 8

struct fifo {
	uint8_t buf[FIFO_CAP];
	size_t head, tail, count;
};

void fifo_init(struct fifo *f);
bool fifo_put(struct fifo *f, uint8_t v);   /* false when full */
bool fifo_get(struct fifo *f, uint8_t *v);  /* false when empty */
size_t fifo_count(const struct fifo *f);
#endif
