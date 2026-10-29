cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(input_keys)
# app.overlay: two gpio-keys buttons on an emulated GPIO controller
target_sources(app PRIVATE src/keys.c src/test_keys.c)
