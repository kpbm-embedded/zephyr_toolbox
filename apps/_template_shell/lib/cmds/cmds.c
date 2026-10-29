#include <errno.h>
#include <stdlib.h>
#include <zephyr/shell/shell.h>
#include "cmds.h"

struct app_ctl app_ctl = {.gain = 10};

static int cmd_status(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	shell_print(sh, "gain=%d running=%d", app_ctl.gain, app_ctl.running);
	return 0;
}

static int cmd_gain(const struct shell *sh, size_t argc, char **argv)
{
	char *end;
	long v = strtol(argv[1], &end, 10);

	ARG_UNUSED(argc);
	if (*end != '\0' || v < 0 || v > 100) {
		shell_error(sh, "gain must be 0..100, got '%s'", argv[1]);
		return -EINVAL;
	}
	app_ctl.gain = (int)v;
	shell_print(sh, "gain set to %d", app_ctl.gain);
	return 0;
}

static int cmd_start(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	if (app_ctl.running) {
		shell_warn(sh, "already running");
		return -EALREADY;
	}
	app_ctl.running = true;
	shell_print(sh, "started");
	return 0;
}

static int cmd_stop(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	app_ctl.running = false;
	shell_print(sh, "stopped");
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(app_cmds,
	SHELL_CMD(status, NULL, "Show gain and run state", cmd_status),
	/* mandatory args: 2 (command + value), optional: 0 -> the shell rejects other counts itself */
	SHELL_CMD_ARG(gain, NULL, "Set gain: app gain <0..100>", cmd_gain, 2, 0),
	SHELL_CMD(start, NULL, "Start", cmd_start),
	SHELL_CMD(stop, NULL, "Stop", cmd_stop),
	SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(app, &app_cmds, "Application commands", NULL);
