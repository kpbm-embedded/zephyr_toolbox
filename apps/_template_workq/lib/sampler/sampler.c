#include <zephyr/sys/ring_buffer.h>
#include "sampler.h"

#define SAMPLES 16

static K_THREAD_STACK_DEFINE(stack, 1024);
static struct k_work_q queue;
static struct k_work_delayable work;
static sampler_read_fn read_fn;
static k_timeout_t period;
static atomic_t dropped;
static bool running;
RING_BUF_DECLARE(samples, SAMPLES * sizeof(int32_t));
static struct k_spinlock lock;

static void sample(struct k_work *w)
{
	int32_t v = read_fn();
	k_spinlock_key_t key = k_spin_lock(&lock);

	if (ring_buf_put(&samples, (uint8_t *)&v, sizeof(v)) != sizeof(v)) {
		atomic_inc(&dropped);
	}
	k_spin_unlock(&lock, key);
	if (running) {
		/* next deadline from now: drift accumulates by the handler time; use k_timer for a strict rate */
		k_work_reschedule_for_queue(&queue, &work, period);
	}
}

void sampler_start(uint32_t period_ms, sampler_read_fn read)
{
	static bool queue_started;

	if (!queue_started) {
		k_work_queue_start(&queue, stack, K_THREAD_STACK_SIZEOF(stack), K_PRIO_PREEMPT(3), NULL);
		k_work_init_delayable(&work, sample);
		queue_started = true;
	}
	read_fn = read;
	period = K_MSEC(period_ms);
	atomic_clear(&dropped);
	ring_buf_reset(&samples);
	running = true;
	k_work_reschedule_for_queue(&queue, &work, period);
}

void sampler_stop(void)
{
	struct k_work_sync sync;

	running = false;
	k_work_cancel_delayable_sync(&work, &sync);
}

size_t sampler_drain(int32_t *out, size_t max)
{
	k_spinlock_key_t key = k_spin_lock(&lock);
	uint32_t n = ring_buf_get(&samples, (uint8_t *)out, max * sizeof(int32_t));

	k_spin_unlock(&lock, key);
	return n / sizeof(int32_t);
}

uint32_t sampler_dropped(void)
{
	return (uint32_t)atomic_get(&dropped);
}
