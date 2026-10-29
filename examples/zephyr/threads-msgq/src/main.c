#include <zephyr/kernel.h>

struct msg { uint32_t seq; int64_t ts; };
K_MSGQ_DEFINE(q, sizeof(struct msg), 8, 4);
K_MUTEX_DEFINE(lock);
static uint32_t total;

static void producer(void *a, void *b, void *c)
{
	for (uint32_t i = 0;; i++) {
		struct msg m = { i, k_uptime_get() };
		if (k_msgq_put(&q, &m, K_MSEC(100)) != 0) {
			printk("queue full, dropped %u\n", i);
		}
		k_msleep(200);
	}
}

static void consumer(void *a, void *b, void *c)
{
	struct msg m;
	while (k_msgq_get(&q, &m, K_FOREVER) == 0) {
		k_mutex_lock(&lock, K_FOREVER);
		total++;
		k_mutex_unlock(&lock);
		printk("got #%u (sent at %lld ms), total %u\n", m.seq, m.ts, total);
	}
}
K_THREAD_DEFINE(prod, 1024, producer, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(cons, 1024, consumer, NULL, NULL, NULL, 6, 0, 0);

int main(void) { return 0; }
