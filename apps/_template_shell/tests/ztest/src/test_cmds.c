#include <string.h>
#include <zephyr/ztest.h>
#include <zephyr/shell/shell.h>
#include <zephyr/shell/shell_dummy.h>
#include "cmds.h"

static const struct shell *sh;

static void *setup(void)
{
	sh = shell_backend_dummy_get_ptr();
	/* the dummy backend starts with the shell thread; wait until it accepts commands */
	WAIT_FOR(shell_ready(sh), 1000000, k_msleep(1));
	return NULL;
}

static void before(void *f)
{
	ARG_UNUSED(f);
	app_ctl.gain = 10;
	app_ctl.running = false;
	shell_backend_dummy_clear_output(sh);
}

ZTEST_SUITE(cmds, NULL, setup, before, NULL, NULL);

static const char *run(const char *cmd, int *ret)
{
	size_t len;

	shell_backend_dummy_clear_output(sh);
	*ret = shell_execute_cmd(sh, cmd);
	return shell_backend_dummy_get_output(sh, &len);
}

ZTEST(cmds, test_status)
{
	int ret;
	const char *out = run("app status", &ret);

	zassert_ok(ret);
	zassert_not_null(strstr(out, "gain=10 running=0"), "output: %s", out);
}

ZTEST(cmds, test_gain_set_and_range)
{
	int ret;

	run("app gain 42", &ret);
	zassert_ok(ret);
	zassert_equal(app_ctl.gain, 42);
	run("app gain 101", &ret);
	zassert_equal(ret, -EINVAL);
	run("app gain x1", &ret);
	zassert_equal(ret, -EINVAL);
	zassert_equal(app_ctl.gain, 42, "rejected values change nothing");
}

ZTEST(cmds, test_wrong_argument_count)
{
	int ret;

	run("app gain", &ret);
	zassert_not_equal(ret, 0, "the shell checks the mandatory argument count");
}

ZTEST(cmds, test_start_twice)
{
	int ret;

	run("app start", &ret);
	zassert_ok(ret);
	run("app start", &ret);
	zassert_equal(ret, -EALREADY);
	zassert_true(app_ctl.running);
}
