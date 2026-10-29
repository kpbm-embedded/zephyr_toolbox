/*
 * Minimal Rockchip GPIO v2 driver (RV1106/RV1103, RK3506, RK3568, RK3588).
 *
 * Register layout (GPIO v2): every 32-bit register covers 16 pins in its low
 * half; the high half is a write-enable mask, so no read-modify-write is needed.
 *   0x00 SWPORT_DR_L   0x04 SWPORT_DR_H    data out, pins 0-15 / 16-31
 *   0x08 SWPORT_DDR_L  0x0c SWPORT_DDR_H   direction (1 = output)
 *   0x70 EXT_PORT                          input level, pins 0-31
 * Pull-up/down and pin muxing live in the IOC/GRF, not in this block: the pin
 * must already be in its GPIO function (reset default for most pins), and pull
 * flags are rejected with -ENOTSUP. No interrupt support.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT rockchip_gpio_bank

#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_utils.h>
#include <zephyr/sys/device_mmio.h>
#include <zephyr/sys/sys_io.h>
#include <zephyr/spinlock.h>

#define RK_GPIO_DR_L   0x00
#define RK_GPIO_DDR_L  0x08
#define RK_GPIO_EXT    0x70

struct gpio_rk_config {
	struct gpio_driver_config common;
	DEVICE_MMIO_ROM;
};

struct gpio_rk_data {
	struct gpio_driver_data common;
	DEVICE_MMIO_RAM;
	uint32_t out; /* shadow of the output latch, for toggle */
	struct k_spinlock lock;
};

/* write bits of 'val' selected by 'mask' (32 pins) to a low/high register pair */
static void rk_write_masked(uintptr_t base, uint32_t reg, uint32_t mask, uint32_t val)
{
	uint32_t lo = mask & 0xffffU;
	uint32_t hi = mask >> 16;

	if (lo != 0U) {
		sys_write32((lo << 16) | (val & lo), base + reg);
	}
	if (hi != 0U) {
		sys_write32((hi << 16) | ((val >> 16) & hi), base + reg + 4);
	}
}

static int gpio_rk_pin_configure(const struct device *dev, gpio_pin_t pin, gpio_flags_t flags)
{
	struct gpio_rk_data *data = dev->data;
	uintptr_t base = DEVICE_MMIO_GET(dev);
	uint32_t bit = BIT(pin);

	if ((flags & (GPIO_PULL_UP | GPIO_PULL_DOWN | GPIO_SINGLE_ENDED)) != 0U) {
		return -ENOTSUP;
	}

	k_spinlock_key_t key = k_spin_lock(&data->lock);

	if ((flags & GPIO_OUTPUT) != 0U) {
		if ((flags & GPIO_OUTPUT_INIT_HIGH) != 0U) {
			data->out |= bit;
		} else if ((flags & GPIO_OUTPUT_INIT_LOW) != 0U) {
			data->out &= ~bit;
		}
		rk_write_masked(base, RK_GPIO_DR_L, bit, data->out);
		rk_write_masked(base, RK_GPIO_DDR_L, bit, bit);
	} else if ((flags & GPIO_INPUT) != 0U) {
		rk_write_masked(base, RK_GPIO_DDR_L, bit, 0);
	}
	k_spin_unlock(&data->lock, key);
	return 0;
}

static int gpio_rk_port_get_raw(const struct device *dev, gpio_port_value_t *value)
{
	*value = sys_read32(DEVICE_MMIO_GET(dev) + RK_GPIO_EXT);
	return 0;
}

static int gpio_rk_port_set_masked_raw(const struct device *dev, gpio_port_pins_t mask,
				       gpio_port_value_t value)
{
	struct gpio_rk_data *data = dev->data;
	k_spinlock_key_t key = k_spin_lock(&data->lock);

	data->out = (data->out & ~mask) | (value & mask);
	rk_write_masked(DEVICE_MMIO_GET(dev), RK_GPIO_DR_L, mask, data->out);
	k_spin_unlock(&data->lock, key);
	return 0;
}

static int gpio_rk_port_set_bits_raw(const struct device *dev, gpio_port_pins_t pins)
{
	return gpio_rk_port_set_masked_raw(dev, pins, pins);
}

static int gpio_rk_port_clear_bits_raw(const struct device *dev, gpio_port_pins_t pins)
{
	return gpio_rk_port_set_masked_raw(dev, pins, 0);
}

static int gpio_rk_port_toggle_bits(const struct device *dev, gpio_port_pins_t pins)
{
	struct gpio_rk_data *data = dev->data;

	return gpio_rk_port_set_masked_raw(dev, pins, ~data->out);
}

static int gpio_rk_pin_interrupt_configure(const struct device *dev, gpio_pin_t pin,
					   enum gpio_int_mode mode, enum gpio_int_trig trig)
{
	return (mode == GPIO_INT_MODE_DISABLED) ? 0 : -ENOTSUP;
}

static DEVICE_API(gpio, gpio_rk_api) = {
	.pin_configure = gpio_rk_pin_configure,
	.port_get_raw = gpio_rk_port_get_raw,
	.port_set_masked_raw = gpio_rk_port_set_masked_raw,
	.port_set_bits_raw = gpio_rk_port_set_bits_raw,
	.port_clear_bits_raw = gpio_rk_port_clear_bits_raw,
	.port_toggle_bits = gpio_rk_port_toggle_bits,
	.pin_interrupt_configure = gpio_rk_pin_interrupt_configure,
};

static int gpio_rk_init(const struct device *dev)
{
	struct gpio_rk_data *data = dev->data;
	uintptr_t base;

	DEVICE_MMIO_MAP(dev, K_MEM_CACHE_NONE);
	base = DEVICE_MMIO_GET(dev);
	/* start from the current latch so pins left on by the boot loader keep their level */
	data->out = (sys_read32(base + RK_GPIO_DR_L) & 0xffffU) |
		    ((sys_read32(base + RK_GPIO_DR_L + 4) & 0xffffU) << 16);
	return 0;
}

#define GPIO_RK_INIT(n)                                                                            \
	static const struct gpio_rk_config gpio_rk_config_##n = {                                  \
		.common = GPIO_COMMON_CONFIG_FROM_DT_INST(n),                                      \
		DEVICE_MMIO_ROM_INIT(DT_DRV_INST(n)),                                              \
	};                                                                                         \
	static struct gpio_rk_data gpio_rk_data_##n;                                               \
	DEVICE_DT_INST_DEFINE(n, gpio_rk_init, NULL, &gpio_rk_data_##n, &gpio_rk_config_##n,       \
			      PRE_KERNEL_1, CONFIG_GPIO_INIT_PRIORITY, &gpio_rk_api);

DT_INST_FOREACH_STATUS_OKAY(GPIO_RK_INIT)
