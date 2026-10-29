#include <zephyr/ztest.h>
#include "debounce.h"

/* --- system work queue vs. an own work queue ------------------------------------------------------ */
static K_THREAD_STACK_DEFINE(own_stack, 1024);
static struct k_work_q own_q;
static k_tid_t ran_on;
static K_SEM_DEFINE(ran, 0, 1);

static void record_thread(struct k_work *w)
{
	ARG_UNUSED(w);
	ran_on = k_current_get();
	k_sem_give(&ran);
}

static void *setup(void)
{
	struct k_work_queue_config cfg = {.name = "own_wq"};

	k_work_queue_start(&own_q, own_stack, K_THREAD_STACK_SIZEOF(own_stack), K_PRIO_PREEMPT(4), &cfg);
	return NULL;
}

static void before(void *f)
{
	ARG_UNUSED(f);
	debounce_init(&own_q);
	k_sem_reset(&ran);
}

ZTEST_SUITE(workq_timers, NULL, setup, before, NULL, NULL);

ZTEST(workq_timers, test_system_queue)
{
	static struct k_work w;

	k_work_init(&w, record_thread);
	zassert_equal(k_work_submit(&w), 1, "1 = queued");
	zassert_ok(k_sem_take(&ran, K_MSEC(100)));
	zassert_equal_ptr(ran_on, k_work_queue_thread_get(&k_sys_work_q));
}

ZTEST(workq_timers, test_own_queue)
{
	static struct k_work w;

	k_work_init(&w, record_thread);
	k_work_submit_to_queue(&own_q, &w);
	zassert_ok(k_sem_take(&ran, K_MSEC(100)));
	zassert_equal_ptr(ran_on, k_work_queue_thread_get(&own_q));
	zassert_str_equal(k_thread_name_get(ran_on), "own_wq");
}

/* --- delayable work: debounce ----------------------------------------------------------------------- */
ZTEST(workq_timers, test_bounces_give_one_report)
{
	for (int i = 0; i < 6; i++) { /* contact bounce: 6 edges 5 ms apart, ends high */
		debounce_edge(i & 1);
		k_msleep(5);
	}
	debounce_edge(1);
	zassert_equal(debounce_reports(), 0, "reported while still bouncing");
	k_msleep(DEBOUNCE_MS + 20);
	zassert_equal(debounce_reports(), 1);
	zassert_equal(debounce_stable_level(), 1);
}

ZTEST(workq_timers, test_cancel_pending)
{
	debounce_edge(1);
	k_msleep(5);
	zassert_true(debounce_cancel(), "true: it was pending; now it is idle");
	k_msleep(DEBOUNCE_MS + 20);
	zassert_equal(debounce_reports(), 0);
}

/* --- kernel timers ---------------------------------------------------------------------------------- */
static atomic_t expiries;
static atomic_t stops;
static void on_expiry(struct k_timer *t) { ARG_UNUSED(t); atomic_inc(&expiries); } /* ISR context */
static void on_stop(struct k_timer *t) { ARG_UNUSED(t); atomic_inc(&stops); }
K_TIMER_DEFINE(tick_timer, on_expiry, on_stop);

ZTEST(workq_timers, test_periodic_timer)
{
	atomic_clear(&expiries);
	k_timer_start(&tick_timer, K_MSEC(10), K_MSEC(10));
	k_msleep(105);
	k_timer_stop(&tick_timer);
	zassert_within(atomic_get(&expiries), 10, 1, "expiries %ld", atomic_get(&expiries));
	zassert_equal(atomic_get(&stops), 1, "stop function runs on k_timer_stop of a running timer");
}

ZTEST(workq_timers, test_one_shot_status_sync)
{
	k_timer_start(&tick_timer, K_MSEC(20), K_NO_WAIT);
	/* blocks until the timer expires; returns the expiries since the last status read */
	zassert_equal(k_timer_status_sync(&tick_timer), 1);
	zassert_equal(k_timer_remaining_get(&tick_timer), 0);
}
