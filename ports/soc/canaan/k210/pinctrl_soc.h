/*
 * Placeholder pinctrl types for the K210 (no FPIOA pinctrl driver yet). Needed because
 * drivers/serial/uart_sifive.c includes <zephyr/drivers/pinctrl.h> unconditionally.
 * UARTHS is already routed to IO4/IO5 by the boot ROM.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef SOC_PORTS_K210_PINCTRL_SOC_H_
#define SOC_PORTS_K210_PINCTRL_SOC_H_

#include <stdint.h>

typedef uint32_t pinctrl_soc_pin_t;

#define Z_PINCTRL_STATE_PIN_INIT(node_id, prop, idx) DT_PROP_BY_IDX(node_id, prop, idx),

#define Z_PINCTRL_STATE_PINS_INIT(node_id, prop)                                                   \
	{DT_FOREACH_PROP_ELEM(node_id, prop, Z_PINCTRL_STATE_PIN_INIT)}

#endif
