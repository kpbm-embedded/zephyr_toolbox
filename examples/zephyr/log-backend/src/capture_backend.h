#ifndef CAPTURE_BACKEND_H
#define CAPTURE_BACKEND_H
#include <zephyr/logging/log_backend.h>

/* LOG_BACKEND_DEFINE makes the backend static: reach it through this function */
const struct log_backend *capture_backend_get(void);
#define capture_backend (capture_backend_get())

void capture_clear(void);
const char *capture_text(void);
int capture_lines(void);

#endif
