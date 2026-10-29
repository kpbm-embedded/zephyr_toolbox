#ifndef SAMPLER_H
#define SAMPLER_H
#include <zephyr/kernel.h>

typedef int32_t (*sampler_read_fn)(void);

void sampler_start(uint32_t period_ms, sampler_read_fn read);
void sampler_stop(void);                   /* waits until a running sample is finished */
size_t sampler_drain(int32_t *out, size_t max); /* oldest first */
uint32_t sampler_dropped(void);             /* samples lost because the buffer was full */

#endif
