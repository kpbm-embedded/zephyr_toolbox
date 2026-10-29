/* Pump controller as a hierarchical state machine (SMF):
 *
 *   IDLE --start--> RUNNING{ PRIME --primed--> PUMP }   RUNNING --stop--> IDLE
 *                   RUNNING --fault--> FAULT --reset--> IDLE           IDLE --shutdown--> (terminate 42)
 *
 * PRIME and PUMP do not handle stop/fault themselves: they propagate the event to their parent RUNNING.
 */
#include <string.h>
#include "pump.h"

static const struct smf_state states[];

static void mark(void *o, const char *tok)
{
	struct pump *p = o;
	size_t n = strlen(p->trace);

	if (n + strlen(tok) + 2 < sizeof(p->trace)) {
		if (n) {
			p->trace[n++] = ' ';
		}
		strcpy(&p->trace[n], tok);
	}
}

static void idle_entry(void *o) { mark(o, "I>"); }
static void idle_exit(void *o) { mark(o, "I<"); }
static enum smf_state_result idle_run(void *o)
{
	struct pump *p = o;

	if (p->ev == EV_START) {
		smf_set_state(SMF_CTX(p), &states[RUNNING]);
	} else if (p->ev == EV_SHUTDOWN) {
		smf_set_terminate(SMF_CTX(p), 42);
	}
	return SMF_EVENT_HANDLED;
}

static void running_entry(void *o) { mark(o, "R>"); }
static void running_exit(void *o) { mark(o, "R<"); }
static enum smf_state_result running_run(void *o)
{
	struct pump *p = o;

	if (p->ev == EV_STOP) {
		smf_set_state(SMF_CTX(p), &states[IDLE]);
	} else if (p->ev == EV_FAULT) {
		smf_set_state(SMF_CTX(p), &states[FAULT]);
	}
	return SMF_EVENT_HANDLED;
}

static void prime_entry(void *o) { mark(o, "P>"); }
static void prime_exit(void *o) { mark(o, "P<"); }
static enum smf_state_result prime_run(void *o)
{
	struct pump *p = o;

	if (p->ev == EV_PRIMED) {
		smf_set_state(SMF_CTX(p), &states[PUMP]);
		return SMF_EVENT_HANDLED;
	}
	return SMF_EVENT_PROPAGATE; /* stop/fault: RUNNING decides */
}

static void pump_entry(void *o) { mark(o, "U>"); }
static void pump_exit(void *o) { mark(o, "U<"); }
static enum smf_state_result pump_run(void *o)
{
	struct pump *p = o;

	p->pump_ticks++;
	return SMF_EVENT_PROPAGATE;
}

static void fault_entry(void *o)
{
	struct pump *p = o;

	p->faults++;
	mark(o, "F>");
}
static void fault_exit(void *o) { mark(o, "F<"); }
static enum smf_state_result fault_run(void *o)
{
	struct pump *p = o;

	if (p->ev == EV_RESET) {
		smf_set_state(SMF_CTX(p), &states[IDLE]);
	}
	return SMF_EVENT_HANDLED;
}

static const struct smf_state states[] = {
	[IDLE] = SMF_CREATE_STATE(idle_entry, idle_run, idle_exit, NULL, NULL),
	[RUNNING] = SMF_CREATE_STATE(running_entry, running_run, running_exit, NULL, &states[PRIME]),
	[PRIME] = SMF_CREATE_STATE(prime_entry, prime_run, prime_exit, &states[RUNNING], NULL),
	[PUMP] = SMF_CREATE_STATE(pump_entry, pump_run, pump_exit, &states[RUNNING], NULL),
	[FAULT] = SMF_CREATE_STATE(fault_entry, fault_run, fault_exit, NULL, NULL),
};

void pump_init(struct pump *p)
{
	memset(p, 0, sizeof(*p));
	smf_set_initial(SMF_CTX(p), &states[IDLE]);
}

int32_t pump_event(struct pump *p, enum pump_event ev)
{
	p->ev = ev;
	return smf_run_state(SMF_CTX(p));
}

const struct smf_state *pump_state(enum pump_state s)
{
	return &states[s];
}
