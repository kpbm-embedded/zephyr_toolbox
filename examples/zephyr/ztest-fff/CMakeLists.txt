cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ztest_fff)
# avg.c is the code under test; sensor_hw_read() is not linked in: the test fakes it with FFF
target_sources(app PRIVATE src/avg.c src/test_avg.c)
