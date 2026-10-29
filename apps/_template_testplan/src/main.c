#include <string.h>
#include <zephyr/kernel.h>
#include "crc.h"

int main(void)
{
	const char *check = "123456789";
	uint16_t crc = crc16_ccitt((const uint8_t *)check, strlen(check));

	printk("__name__ selftest\n");
	printk("crc %s = 0x%04x\n", check, crc);
	printk("result: %s\n", crc == 0x29b1 ? "PASS" : "FAIL");
	return 0;
}
