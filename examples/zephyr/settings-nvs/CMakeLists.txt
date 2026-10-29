cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(settings_nvs)
# app.overlay (picked up automatically) adds a simulated flash with a storage partition
target_sources(app PRIVATE src/config.c src/test_settings.c)
