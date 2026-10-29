# zephyr / usb-cdc

USB device CDC ACM serial port (new device stack) that echoes characters back.

Status: builds; untested on hardware. Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`); per-board changes are devicetree overlays in `boards/`, nothing in the Zephyr tree is edited.

## Boards and pins
USB data connector: BlackPill USB-C PA11 D-/PA12 D+; WeAct G431 PA11/PA12; mini H743 PA11/PA12; Pico/Pico 2/Pro Micro: native USB. All via `zephyr_udc0` in each board DTS. Appears as /dev/ttyACM*.

## Build / flash / monitor
```
. ../../../toolchain/zephyr/env.sh
west build -p auto -b <west-board> -d $ZEPHYR_OUT/examples/usb-cdc/<west-board> .
../../../scripts/app example usb-cdc <west-board>   # the same build via this ID's scripts/app
west flash -d $ZEPHYR_OUT/examples/usb-cdc/<west-board>   # uf2 boards: copy zephyr/zephyr.uf2 to the bootloader drive
../../../scripts/app monitor /dev/ttyACM0 115200   # or /dev/ttyUSB0 with a USB-UART on the console pins
```
West boards per target: blackpill_f411ce, weact_g431=weact_stm32g431_core, weact_h562=weact_stm32h562_core, weact_h743=mini_stm32h743, rpi_pico, rpi_pico2=rpi_pico2/rp2350a/m33, promicro_nrf52840=promicro_nrf52840/nrf52840/uf2, esp32c3=esp32c3_devkitm, esp32s3=esp32s3_devkitc/esp32s3/procpu, ch32v003=ch32v003evt.

## Expected output
After enumeration the port prints `usb-cdc up: type to echo` and echoes input.

## Pin sources
zephyr/ws/zephyr/samples/subsys/usb/cdc_acm (app.overlay, prj.conf) and samples/subsys/usb/common (sample_usbd).
