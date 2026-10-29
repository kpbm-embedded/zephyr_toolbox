#include "fsm.h"

static const struct smf_state states[ST_COUNT];

static enum smf_state_result boot_run(void *o)
{
	struct fsm *f = o;

	if (f->ev == EV_BOOTED) {
		smf_set_state(SMF_CTX(f), &states[ST_IDLE]);
	}
	return SMF_EVENT_HANDLED;
}

static enum smf_state_result idle_run(void *o)
{
	struct fsm *f = o;

	if (f->ev == EV_START) {
		smf_set_state(SMF_CTX(f), &states[ST_ACTIVE]);
	} else if (f->ev == EV_FAIL) {
		smf_set_state(SMF_CTX(f), &states[ST_ERROR]);
	}
	return SMF_EVENT_HANDLED;
}

static void active_entry(void *o)
{
	((struct fsm *)o)->active_runs++;
}

static enum smf_state_result active_run(void *o)
{
	struct fsm *f = o;

	if (f->ev == EV_STOP) {
		smf_set_state(SMF_CTX(f), &states[ST_IDLE]);
	} else if (f->ev == EV_FAIL) {
		smf_set_state(SMF_CTX(f), &states[ST_ERROR]);
	}
	return SMF_EVENT_HANDLED;
}

static void error_entry(void *o)
{
	((struct fsm *)o)->last_error = -EIO;
}

static enum smf_state_result error_run(void *o)
{
	struct fsm *f = o;

	if (f->ev == EV_RESET) {
		f->last_error = 0;
		smf_set_state(SMF_CTX(f), &states[ST_IDLE]);
	}
	return SMF_EVENT_HANDLED;
}

static const struct smf_state states[ST_COUNT] = {
	[ST_BOOT] = SMF_CREATE_STATE(NULL, boot_run, NULL, NULL, NULL),
	[ST_IDLE] = SMF_CREATE_STATE(NULL, idle_run, NULL, NULL, NULL),
	[ST_ACTIVE] = SMF_CREATE_STATE(active_entry, active_run, NULL, NULL, NULL),
	[ST_ERROR] = SMF_CREATE_STATE(error_entry, error_run, NULL, NULL, NULL),
};

void fsm_init(struct fsm *f)
{
	*f = (struct fsm){0};
	smf_set_initial(SMF_CTX(f), &states[ST_BOOT]);
}

void fsm_step(struct fsm *f, enum fsm_event ev)
{
	f->ev = ev;
	smf_run_state(SMF_CTX(f));
}

enum fsm_state fsm_current(const struct fsm *f)
{
	return (enum fsm_state)(f->ctx.current - &states[0]);
}

const char *fsm_name(enum fsm_state s)
{
	static const char *const names[ST_COUNT] = {"BOOT", "IDLE", "ACTIVE", "ERROR"};

	return s < ST_COUNT ? names[s] : "?";
}
