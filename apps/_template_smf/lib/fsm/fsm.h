#ifndef FSM_H
#define FSM_H
#include <zephyr/kernel.h>
#include <zephyr/smf.h>

enum fsm_state { ST_BOOT, ST_IDLE, ST_ACTIVE, ST_ERROR, ST_COUNT };
enum fsm_event { EV_BOOTED, EV_START, EV_STOP, EV_FAIL, EV_RESET };

struct fsm {
	struct smf_ctx ctx; /* first member: SMF_CTX() */
	enum fsm_event ev;
	uint32_t active_runs;
	int last_error;
};

void fsm_init(struct fsm *f);
/* run one event through the machine (call from one thread only) */
void fsm_step(struct fsm *f, enum fsm_event ev);
enum fsm_state fsm_current(const struct fsm *f);
const char *fsm_name(enum fsm_state s);

#endif
