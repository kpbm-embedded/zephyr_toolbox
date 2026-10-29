#include <zephyr/kernel.h>
#include "datalog.h"

int main(void)
{
	for (int i = 0; i < 20; i++) {
		int err = datalog_append(i * 10);

		if (err) {
			printk("append failed: %d (is " DATALOG_DIR " mounted?)\n", err);
			return 0;
		}
		k_msleep(100);
	}
	printk("__name__: %d records, %u rotations\n", datalog_count(), datalog_rotations());
	return 0;
}
