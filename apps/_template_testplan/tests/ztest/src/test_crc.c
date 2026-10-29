#include <string.h>
#include <zephyr/ztest.h>
#include "crc.h"

ZTEST_SUITE(crc, NULL, NULL, NULL, NULL, NULL);

ZTEST(crc, test_check_value)
{
	const char *s = "123456789";

	zassert_equal(crc16_ccitt((const uint8_t *)s, strlen(s)), 0x29b1);
}

ZTEST(crc, test_empty_is_init)
{
	zassert_equal(crc16_ccitt(NULL, 0), 0xffff);
}

ZTEST(crc, test_single_bit_change_detected)
{
	uint8_t a[8] = {1, 2, 3, 4, 5, 6, 7, 8}, b[8];

	memcpy(b, a, sizeof(a));
	b[5] ^= 0x10;
	zassert_not_equal(crc16_ccitt(a, sizeof(a)), crc16_ccitt(b, sizeof(b)));
}

ZTEST(crc, test_variant_config)
{
	TC_PRINT("asserts %s, speed opt %s, riscv %s\n", IS_ENABLED(CONFIG_ASSERT) ? "on" : "off",
		 IS_ENABLED(CONFIG_SPEED_OPTIMIZATIONS) ? "on" : "off", IS_ENABLED(CONFIG_RISCV) ? "yes" : "no");
	if (!IS_ENABLED(CONFIG_RISCV) && !IS_ENABLED(CONFIG_ARM)) {
		ztest_test_skip(); /* a skip is reported as such, not as a pass */
	}
}
