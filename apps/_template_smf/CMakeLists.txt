# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(__name__)

target_include_directories(app PRIVATE lib/fsm)
target_sources(app PRIVATE src/main.c lib/fsm/fsm.c)
