# SPDX-License-Identifier: Apache-2.0
if(CONFIG_THEAD_CLINT_TIMER)
  zephyr_library_named(soc_ports_timer)
  zephyr_library_sources(thead_clint_timer.c)
endif()
