#ifndef DEBOUNCE_H
#define DEBOUNCE_H
#include <zephyr/kernel.h>

#define DEBOUNCE_MS 30

/* A button debouncer on a delayable work item: every edge restarts the 30 ms window; the level is
 * taken as stable (and reported once) only when no edge came for 30 ms.
 */
void debounce_init(struct k_work_q *queue);
void debounce_edge(int level);       /* call from the GPIO interrupt */
int debounce_stable_level(void);
int debounce_reports(void);
bool debounce_cancel(void);          /* drop a pending report (e.g. on sleep); true if one was pending */

#endif
