# SPDX-License-Identifier: Apache-2.0
zephyr_include_directories(.)
zephyr_sources(${CMAKE_CURRENT_LIST_DIR}/../../common/armv7a_uboot_go.c)
set(SOC_LINKER_SCRIPT ${ZEPHYR_BASE}/include/zephyr/arch/arm/cortex_a_r/scripts/linker.ld CACHE INTERNAL "")
