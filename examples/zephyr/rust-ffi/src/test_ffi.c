#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/ztest.h>
#include "zffi.h"

/* called by the Rust panic handler */
FUNC_NORETURN void k_panic_from_rust(void)
{
	k_panic();
	CODE_UNREACHABLE;
}

ZTEST_SUITE(rust_ffi, NULL, NULL, NULL, NULL, NULL);

ZTEST(rust_ffi, test_crc16)
{
	const char *s = "123456789";

	zassert_equal(zffi_crc16((const uint8_t *)s, strlen(s)), 0x29b1);
	zassert_equal(zffi_crc16(NULL, 5), 0xffff, "NULL is handled on the Rust side");
}

ZTEST(rust_ffi, test_checked_add)
{
	uint32_t out = 7;

	zassert_true(zffi_checked_add(40, 2, &out));
	zassert_equal(out, 42);
	zassert_false(zffi_checked_add(UINT32_MAX, 1, &out));
	zassert_equal(out, 42, "untouched on overflow");
}

ZTEST(rust_ffi, test_pi_struct_shared_with_c)
{
	struct zffi_pi pi = {.kp = 2.0f, .ki = 1.0f, .out_min = -10.0f, .out_max = 10.0f};

	/* into a variable first: zassert_within() evaluates its arguments more than once */
	float out = zffi_pi_step(&pi, 1.0f, 0.5f);

	zassert_within(out, 2.5f, 0.001f);
	zassert_within(pi.integral, 0.5f, 0.001f, "Rust wrote the C struct");
	out = zffi_pi_step(&pi, 100.0f, 0.5f);
	zassert_equal(out, 10.0f, "clamped");
	zassert_within(pi.integral, 0.5f, 0.001f, "no wind-up while saturated");
}
