#include "fifo.h"

void fifo_init(struct fifo *f)
{
	f->head = f->tail = f->count = 0;
}

bool fifo_put(struct fifo *f, uint8_t v)
{
	if (f->count == FIFO_CAP) {
		return false;
	}
	f->buf[f->head] = v;
	f->head = (f->head + 1) % FIFO_CAP;
	f->count++;
	return true;
}

bool fifo_get(struct fifo *f, uint8_t *v)
{
	if (f->count == 0) {
		return false;
	}
	*v = f->buf[f->tail];
	f->tail = (f->tail + 1) % FIFO_CAP;
	f->count--;
	return true;
}

size_t fifo_count(const struct fifo *f)
{
	return f->count;
}
