#include <string.h>
#include <zephyr/ztest.h>
#include "fixed.h"

ZTEST_SUITE(zig_ffi, NULL, NULL, NULL, NULL, NULL);

#define Q16(x) ((int32_t)((x) * 65536))

ZTEST(zig_ffi, test_q16)
{
	zassert_equal(zf_q16_from_milli(3, 250), Q16(3.25));
	zassert_equal(zf_q16_mul(Q16(1.5), Q16(-2)), Q16(-3));
	zassert_equal(zf_q16_mul(Q16(30000), Q16(30000)), INT32_MAX, "saturates");
}

ZTEST(zig_ffi, test_fletcher16)
{
	const char *s = "abcde";

	zassert_equal(zf_fletcher16((const uint8_t *)s, strlen(s)), 0xc8f0); /* reference value */
	zassert_equal(zf_fletcher16((const uint8_t *)s, 0), 0);
}

ZTEST(zig_ffi, test_struct_shared_with_c)
{
	struct zf_avg avg = {0};
	int32_t r = 0;

	for (int i = 1; i <= 6; i++) {
		r = zf_avg_add(&avg, i * 10);
	}
	zassert_equal(r, 45, "average of 30..60"); /* (30+40+50+60)/4 */
	zassert_equal(avg.n, 4);
}
