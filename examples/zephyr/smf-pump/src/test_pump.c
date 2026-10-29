#include <string.h>
#include <zephyr/ztest.h>
#include "pump.h"

static struct pump p;

static void before(void *f)
{
	ARG_UNUSED(f);
	pump_init(&p);
}

ZTEST_SUITE(smf_pump, NULL, NULL, before, NULL, NULL);

static void clear_trace(void)
{
	p.trace[0] = '\0';
}

ZTEST(smf_pump, test_initial_state_entered)
{
	zassert_str_equal(p.trace, "I>");
	zassert_equal_ptr(p.ctx.current, pump_state(IDLE));
}

ZTEST(smf_pump, test_start_enters_parent_then_initial_child)
{
	clear_trace();
	zassert_equal(pump_event(&p, EV_START), 0);
	zassert_str_equal(p.trace, "I< R> P>");
	zassert_equal_ptr(p.ctx.current, pump_state(PRIME));
}

ZTEST(smf_pump, test_sibling_transition_keeps_parent)
{
	pump_event(&p, EV_START);
	clear_trace();
	pump_event(&p, EV_PRIMED);
	zassert_str_equal(p.trace, "P< U>", "RUNNING must not be exited: %s", p.trace);
}

ZTEST(smf_pump, test_child_propagates_stop_to_parent)
{
	pump_event(&p, EV_START);
	pump_event(&p, EV_PRIMED);
	clear_trace();
	pump_event(&p, EV_STOP);
	zassert_str_equal(p.trace, "U< R< I>");
	zassert_equal(p.pump_ticks, 1, "PUMP's run() saw the event before propagating it");
}

ZTEST(smf_pump, test_fault_and_reset)
{
	pump_event(&p, EV_START);
	clear_trace();
	pump_event(&p, EV_FAULT);
	zassert_str_equal(p.trace, "P< R< F>");
	zassert_equal(p.faults, 1);
	pump_event(&p, EV_START); /* ignored in FAULT */
	zassert_equal_ptr(p.ctx.current, pump_state(FAULT));
	pump_event(&p, EV_RESET);
	zassert_equal_ptr(p.ctx.current, pump_state(IDLE));
}

ZTEST(smf_pump, test_unhandled_event_changes_nothing)
{
	clear_trace();
	pump_event(&p, EV_PRIMED);
	zassert_str_equal(p.trace, "");
	zassert_equal_ptr(p.ctx.current, pump_state(IDLE));
}

ZTEST(smf_pump, test_terminate_returns_value)
{
	zassert_equal(pump_event(&p, EV_SHUTDOWN), 42);
}
