#include <zephyr/kernel.h>

/* the shell runs in its own thread on the console UART; the commands are in lib/cmds */
int main(void)
{
	printk("__name__ ready: try 'app status'\n");
	return 0;
}
