# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.20.0)
# this module (two levels up) next to Zephyr's own modules
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_LIST_DIR}/../..)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(__name___tests)

target_sources(app PRIVATE src/test_tally.c)
