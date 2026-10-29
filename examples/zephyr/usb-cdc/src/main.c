#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usbd.h>
#include <sample_usbd.h>

static const struct device *const cdc = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);

int main(void)
{
	struct usbd_context *ctx = sample_usbd_init_device(NULL);
	if (ctx == NULL || usbd_enable(ctx) != 0 || !device_is_ready(cdc)) {
		return 0;
	}
	const char *hello = "usb-cdc up: type to echo\r\n";
	for (const char *p = hello; *p; p++) { uart_poll_out(cdc, *p); }
	while (1) {
		unsigned char c;
		if (uart_poll_in(cdc, &c) == 0) {
			uart_poll_out(cdc, c);
		} else {
			k_msleep(5);
		}
	}
}
