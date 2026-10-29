cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(can_loopback)
# app.overlay: Zephyr's software CAN loopback controller as zephyr,canbus
target_sources(app PRIVATE src/proto.c src/test_can.c)
