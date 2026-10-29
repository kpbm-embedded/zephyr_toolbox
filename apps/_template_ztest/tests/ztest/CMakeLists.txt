# SPDX-License-Identifier: Apache-2.0
# ztest image: the app's lib/ plus the test cases (no src/main.c: ztest brings its own main).
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ztest_app_tests)

set(LIB ${CMAKE_CURRENT_LIST_DIR}/../../lib)
target_include_directories(app PRIVATE ${LIB}/counter)
target_sources(app PRIVATE ${LIB}/counter/counter.c src/test_counter.c)
