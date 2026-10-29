#ifndef PUMP_H
#define PUMP_H
#include <zephyr/smf.h>

enum pump_event { EV_NONE, EV_START, EV_PRIMED, EV_STOP, EV_FAULT, EV_RESET, EV_SHUTDOWN };
enum pump_state { IDLE, RUNNING, PRIME, PUMP, FAULT };

struct pump {
	struct smf_ctx ctx; /* must be first: SMF_CTX() casts the object */
	enum pump_event ev;
	char trace[160];    /* "I> I< R> P> ..." entry (>) and exit (<) of each state, in order */
	int faults;
	int pump_ticks;
};

void pump_init(struct pump *p);
int32_t pump_event(struct pump *p, enum pump_event ev); /* != 0: machine terminated with that value */
const struct smf_state *pump_state(enum pump_state s);

#endif
