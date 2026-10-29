/*
 * Shared early init for the Cortex-A7 SoCs of this module (T113, RV1106/RV1103,
 * RK3506) when Zephyr is started from U-Boot with "go <addr>".
 *
 * U-Boot leaves the MMU, caches and its own VBAR enabled. soc_reset_hook() runs
 * before Zephyr's MMU setup: it cleans the D-cache, turns MMU/caches/alignment
 * checks off, invalidates TLB/I-cache/branch predictor and points VBAR at
 * Zephyr's vector table. relocate_vector_table() is overridden so nothing is
 * copied to address 0 (on these SoCs address 0 is either ROM or not where the
 * image lives).
 *
 * Based on the RK3506 code of Zephyr PR #110923 (Apache-2.0) and the RV1106 SoC
 * code in Zephyr main.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include <cmsis_core.h>
#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/linker/linker-defs.h>
#include <zephyr/sys/barrier.h>
#include <zephyr/sys/util.h>
#include <zephyr/arch/arm/mmu/arm_mmu.h>

#define SCTLR_BOOT_STALE_BITS (SCTLR_M_Msk | SCTLR_A_Msk | SCTLR_C_Msk | SCTLR_I_Msk | HIVECS)

static void set_vector_base(void)
{
	uint32_t sctlr = __get_SCTLR();

	sctlr &= ~HIVECS;
	__set_SCTLR(sctlr);
	barrier_isync_fence_full();

	__set_VBAR((uint32_t)_vector_start & VBAR_MASK);
	barrier_isync_fence_full();
}

void soc_reset_hook(void)
{
	uint32_t sctlr = __get_SCTLR();

	if ((sctlr & SCTLR_C_Msk) != 0U) {
		L1C_CleanInvalidateDCacheAll();
	}

	barrier_dsync_fence_full();
	sctlr &= ~SCTLR_BOOT_STALE_BITS;
	__set_SCTLR(sctlr);
	barrier_isync_fence_full();

	__set_TLBIALL(0);
	__set_ICIALLU(0);
	__set_BPIALL(0);
	barrier_dsync_fence_full();
	barrier_isync_fence_full();

	set_vector_base();
}

void relocate_vector_table(void)
{
	set_vector_base();
}

#ifdef CONFIG_ARM_AARCH32_MMU
/* GIC distributor + CPU interface are used through fixed addresses: map them flat. */
static const struct arm_mmu_region mmu_regions[] = {
	MMU_REGION_FLAT_ENTRY("vectors", POINTER_TO_UINT(_vector_start), 0x1000,
			      MT_STRONGLY_ORDERED | MPERM_R | MPERM_X),
	MMU_REGION_FLAT_ENTRY("gic", DT_REG_ADDR_BY_IDX(DT_NODELABEL(gic), 0), 0x3000,
			      MT_STRONGLY_ORDERED | MPERM_R | MPERM_W),
	MMU_REGION_DT_COMPAT_FOREACH_FLAT_ENTRY(ns16550, MT_DEVICE | MPERM_R | MPERM_W)
};

const struct arm_mmu_config mmu_config = {
	.num_regions = ARRAY_SIZE(mmu_regions),
	.mmu_regions = mmu_regions,
};
#endif
