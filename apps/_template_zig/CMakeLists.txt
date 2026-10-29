# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(__name__)

include(zig/zig.cmake)
zig_object(app ${CMAKE_CURRENT_SOURCE_DIR}/zig/filter.zig)
target_include_directories(app PRIVATE zig)
target_sources(app PRIVATE src/main.c)
