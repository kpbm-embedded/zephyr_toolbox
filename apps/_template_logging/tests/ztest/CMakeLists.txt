# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(__name___tests)

set(LIB ${CMAKE_CURRENT_LIST_DIR}/../../lib)
target_include_directories(app PRIVATE ${LIB}/core)
target_sources(app PRIVATE ${LIB}/core/core.c src/test_core.c)
