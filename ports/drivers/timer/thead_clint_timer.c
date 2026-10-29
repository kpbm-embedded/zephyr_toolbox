/*
 * System timer for T-Head C906/C908 cores (Sophgo CV18xx, Canaan K230).
 *
 * Derived from Zephyr v4.4 drivers/timer/riscv_machine_timer.c
 * (Copyright (c) 2024 MASSDRIVER EI, 2018-2023 Intel Corporation).
 * Differences: the T-Head CLINT has no memory-mapped mtime (the counter is read
 * with the time CSR) and its mtimecmp is written as two 32-bit halves.
 * The approach follows the unmerged Zephyr PR #69594 (Sophgo CV18xx).
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <limits.h>

#include <zephyr/init.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/timer/system_timer.h>
#include <zephyr/sys_clock.h>
#include <zephyr/irq.h>
#include <zephyr/arch/riscv/csr.h>

#define DT_DRV_COMPAT thead_c900_clint_mtimer

#define MTIMECMP_REG DT_INST_REG_ADDR_BY_NAME(0, mtimecmp)
#define TIMER_IRQN   DT_INST_IRQN(0)

#define CYC_PER_TICK (uint32_t)(sys_clock_hw_cycles_per_sec() / CONFIG_SYS_CLOCK_TICKS_PER_SEC)

#define cycle_diff_t   unsigned long
#define CYCLE_DIFF_MAX (~(cycle_diff_t)0)

#define CYCLES_MAX_1 ((uint64_t)INT32_MAX * (uint64_t)CYC_PER_TICK)
#define CYCLES_MAX_2 ((uint64_t)CYCLE_DIFF_MAX)
#define CYCLES_MAX_3 MIN(CYCLES_MAX_1, CYCLES_MAX_2)
#define CYCLES_MAX_4 (CYCLES_MAX_3 / 2 + CYCLES_MAX_3 / 4)
#define CYCLES_MAX   (CYCLES_MAX_4 + LSB_GET(CYCLES_MAX_4))

static uint64_t last_count;
static uint64_t last_ticks;
static uint32_t last_elapsed;

#if defined(CONFIG_TEST)
const int32_t z_sys_timer_irq_for_test = TIMER_IRQN;
#endif

static uintptr_t get_hart_mtimecmp(void)
{
	return MTIMECMP_REG + (arch_proc_id() * 8);
}

static void set_mtimecmp(uint64_t time)
{
	volatile uint32_t *r = (uint32_t *)get_hart_mtimecmp();

	/* not latched for multiword writes: park the high word first */
	r[1] = 0xffffffff;
	r[0] = (uint32_t)time;
	r[1] = (uint32_t)(time >> 32);
}

static uint64_t mtime(void)
{
	return csr_read(time);
}

static void timer_isr(const void *arg)
{
	ARG_UNUSED(arg);

	k_spinlock_key_t key = sys_clock_lock();

	uint64_t now = mtime();
	uint64_t dcycles = now - last_count;
	uint32_t dticks = (cycle_diff_t)dcycles / CYC_PER_TICK;

	last_count += (cycle_diff_t)dticks * CYC_PER_TICK;
	last_ticks += dticks;
	last_elapsed = 0;

	if (!IS_ENABLED(CONFIG_TICKLESS_KERNEL)) {
		uint64_t next = last_count + CYC_PER_TICK;

		set_mtimecmp(next);
	}

	sys_clock_announce_locked(dticks, key);
}

void sys_clock_set_timeout(int32_t ticks, bool idle)
{
	ARG_UNUSED(idle);

	if (!IS_ENABLED(CONFIG_TICKLESS_KERNEL)) {
		return;
	}

	uint64_t cyc;

	if (ticks == K_TICKS_FOREVER) {
		cyc = last_count + CYCLES_MAX;
	} else {
		cyc = (last_ticks + last_elapsed + ticks) * CYC_PER_TICK;
		if ((cyc - last_count) > CYCLES_MAX) {
			cyc = last_count + CYCLES_MAX;
		}
	}
	set_mtimecmp(cyc);
}

uint32_t sys_clock_elapsed(void)
{
	if (!IS_ENABLED(CONFIG_TICKLESS_KERNEL)) {
		return 0;
	}

	uint64_t now = mtime();
	uint64_t dcycles = now - last_count;
	uint32_t dticks = (cycle_diff_t)dcycles / CYC_PER_TICK;

	last_elapsed = dticks;
	return dticks;
}

uint32_t sys_clock_cycle_get_32(void)
{
	return (uint32_t)mtime();
}

uint64_t sys_clock_cycle_get_64(void)
{
	return mtime();
}

static int sys_clock_driver_init(void)
{
	IRQ_CONNECT(TIMER_IRQN, 0, timer_isr, NULL, 0);
	last_ticks = mtime() / CYC_PER_TICK;
	last_count = last_ticks * CYC_PER_TICK;
	set_mtimecmp(last_count + CYC_PER_TICK);
	irq_enable(TIMER_IRQN);
	return 0;
}

SYS_INIT(sys_clock_driver_init, PRE_KERNEL_2, CONFIG_SYSTEM_CLOCK_INIT_PRIORITY);
