cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(dt_kconfig)
# app.overlay adds nodes of this app's own binding (dts/bindings/acme,thermostat.yaml: the app dir is a
# DTS_ROOT); Kconfig adds app options next to Zephyr's
target_sources(app PRIVATE src/zones.c src/test_config.c)
