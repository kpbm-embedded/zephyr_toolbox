cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(workq_timers)
target_sources(app PRIVATE src/debounce.c src/test_workq.c)
