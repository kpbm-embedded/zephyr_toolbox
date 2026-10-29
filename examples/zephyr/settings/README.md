# zephyr / settings

Boot counter persisted with the Settings subsystem on the NVS flash backend.

Status: builds; untested on hardware. Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`); per-board changes are devicetree overlays in `boards/`, nothing in the Zephyr tree is edited.

## Boards and pins
No pins; uses the board's `storage_partition` in internal flash (nRF52840, WeAct G431 and ESP32-C3 DTS; Pico/Pico 2 DTS have none, so they are not targets). The Zephyr UF2/flash runner erases storage on full chip erase.

## Build / flash / monitor
```
. ../../../toolchain/zephyr/env.sh
west build -p auto -b <west-board> -d $ZEPHYR_OUT/examples/settings/<west-board> .
../../../scripts/app example settings <west-board>   # the same build via this ID's scripts/app
west flash -d $ZEPHYR_OUT/examples/settings/<west-board>   # uf2 boards: copy zephyr/zephyr.uf2 to the bootloader drive
../../../scripts/app monitor /dev/ttyACM0 115200   # or /dev/ttyUSB0 with a USB-UART on the console pins
```
West boards per target: blackpill_f411ce, weact_g431=weact_stm32g431_core, weact_h562=weact_stm32h562_core, weact_h743=mini_stm32h743, rpi_pico, rpi_pico2=rpi_pico2/rp2350a/m33, promicro_nrf52840=promicro_nrf52840/nrf52840/uf2, esp32c3=esp32c3_devkitm, esp32s3=esp32s3_devkitc/esp32s3/procpu, ch32v003=ch32v003evt.

## Expected output
`boot count N`, N increments on each reset.

## Pin sources
Pattern from zephyr/ws/zephyr/samples/subsys/settings; partitions from each board DTS.
