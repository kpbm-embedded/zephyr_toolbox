cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(littlefs_flashsim)
# app.overlay: simulated flash + a devicetree fstab entry that mounts LittleFS at /lfs on boot
target_sources(app PRIVATE src/test_lfs.c)
