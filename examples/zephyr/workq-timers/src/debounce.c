#include "debounce.h"

static struct k_work_delayable dwork;
static struct k_work_q *wq;
static atomic_t raw_level;
static atomic_t stable_level;
static atomic_t reports;

static void settle(struct k_work *work)
{
	ARG_UNUSED(work);
	atomic_set(&stable_level, atomic_get(&raw_level));
	atomic_inc(&reports);
}

void debounce_init(struct k_work_q *queue)
{
	wq = queue;
	k_work_init_delayable(&dwork, settle);
	atomic_clear(&reports);
	atomic_set(&stable_level, -1);
}

void debounce_edge(int level)
{
	atomic_set(&raw_level, level);
	/* reschedule: (re)start the delay even if already pending (schedule would keep the first deadline) */
	k_work_reschedule_for_queue(wq, &dwork, K_MSEC(DEBOUNCE_MS));
}

int debounce_stable_level(void)
{
	return (int)atomic_get(&stable_level);
}

int debounce_reports(void)
{
	return (int)atomic_get(&reports);
}

bool debounce_cancel(void)
{
	struct k_work_sync sync;

	return k_work_cancel_delayable_sync(&dwork, &sync);
}
