# SPDX-License-Identifier: Apache-2.0
# Application-SoC app template of 32_03_zephyr (copy with: scripts/app new <name> --board <board>).
cmake_minimum_required(VERSION 3.20.0)
# >>> mcu/clion: host unit tests, CMake profile "Host-Tests" (see README.md)
if(MCU_HOST_TESTS)
  project(_template_soc_host_tests C CXX)
  enable_testing()
  add_subdirectory(tests)
  return()
endif()
# <<< mcu/clion
# Out-of-tree boards/SoCs of this ID (ports/ is a Zephyr module): found relative to this app,
# so CLion and plain cmake work without extra options. REALPATH: also correct through symlinks.
# (an app outside apps/ gets ports/ from scripts/app, which exports ZEPHYR_EXTRA_MODULES)
get_filename_component(SOC_PORTS "${CMAKE_CURRENT_LIST_DIR}/../../ports" REALPATH)
if(EXISTS "${SOC_PORTS}/zephyr/module.yml")
  list(APPEND ZEPHYR_EXTRA_MODULES "${SOC_PORTS}")
endif()
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(soc_app)

target_sources(app PRIVATE src/main.c)
