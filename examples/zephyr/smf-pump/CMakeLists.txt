cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(smf_pump)
target_sources(app PRIVATE src/pump.c src/test_pump.c)
