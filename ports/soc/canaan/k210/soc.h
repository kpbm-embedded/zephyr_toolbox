/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SOC_PORTS_K210_SOC_H_
#define SOC_PORTS_K210_SOC_H_

#include <zephyr/devicetree.h>

/* UARTHS is clocked from the CPU clock (390 MHz after the boot ROM); used by uart_sifive.c */
#define SIFIVE_PERIPHERAL_CLOCK_FREQUENCY DT_PROP(DT_NODELABEL(uarths0), clock_frequency)

#endif
