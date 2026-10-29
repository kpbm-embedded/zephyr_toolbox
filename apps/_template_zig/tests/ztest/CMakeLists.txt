# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(__name___tests)

set(APP ${CMAKE_CURRENT_LIST_DIR}/../..)
include(${APP}/zig/zig.cmake)
zig_object(app ${APP}/zig/filter.zig)
target_include_directories(app PRIVATE ${APP}/zig)
target_sources(app PRIVATE src/test_filter.c)
