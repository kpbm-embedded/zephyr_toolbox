cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ztest_basic)
# the code under test (fifo.c) and its test suite (test_fifo.c) in one image
target_sources(app PRIVATE src/fifo.c src/test_fifo.c)
