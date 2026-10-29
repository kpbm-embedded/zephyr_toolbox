/* A log backend that formats messages into a RAM buffer (what a flash/BLE/network backend would send). */
#include <string.h>
#include <zephyr/logging/log_backend.h>
#include <zephyr/logging/log_output.h>
#include "capture_backend.h"

static char text[1024];
static size_t used;
static int lines;

static int out(uint8_t *data, size_t length, void *ctx)
{
	ARG_UNUSED(ctx);
	for (size_t i = 0; i < length && used < sizeof(text) - 1; i++) {
		text[used++] = (char)data[i];
		lines += data[i] == '\n';
	}
	text[used] = '\0';
	return (int)length;
}

static uint8_t out_buf[96];
LOG_OUTPUT_DEFINE(capture_output, out, out_buf, sizeof(out_buf));

static void process(const struct log_backend *const backend, union log_msg_generic *msg)
{
	ARG_UNUSED(backend);
	/* level tag, no timestamp, no colors: "<wrn> sensor: near limit 95" */
	log_output_msg_process(&capture_output, &msg->log, LOG_OUTPUT_FLAG_LEVEL);
}

static void panic(const struct log_backend *const backend)
{
	ARG_UNUSED(backend);
	log_output_flush(&capture_output);
}

static const struct log_backend_api api = {
	.process = process,
	.panic = panic,
};

LOG_BACKEND_DEFINE(capture_backend_ref, api, true);

const struct log_backend *capture_backend_get(void)
{
	return &capture_backend_ref;
}

void capture_clear(void)
{
	used = 0;
	lines = 0;
	text[0] = '\0';
}

const char *capture_text(void)
{
	return text;
}

int capture_lines(void)
{
	return lines;
}
