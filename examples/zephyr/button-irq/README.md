# zephyr / button-irq

GPIO edge interrupt on `sw0` toggles `led0` (callback in ISR context).

Status: builds; untested on hardware. Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`); per-board changes are devicetree overlays in `boards/`, nothing in the Zephyr tree is edited.

## Boards and pins
sw0/led0: BlackPill KEY PA0 (pull-up, active low) / PC13; WeAct G431 PC13 / PC6; WeAct H562 PC13 / PB2; mini H743 PC13 / PE3 (K1 button).

## Build / flash / monitor
```
. ../../../toolchain/zephyr/env.sh
west build -p auto -b <west-board> -d $ZEPHYR_OUT/examples/button-irq/<west-board> .
../../../scripts/app example button-irq <west-board>   # the same build via this ID's scripts/app
west flash -d $ZEPHYR_OUT/examples/button-irq/<west-board>   # uf2 boards: copy zephyr/zephyr.uf2 to the bootloader drive
../../../scripts/app monitor /dev/ttyACM0 115200   # or /dev/ttyUSB0 with a USB-UART on the console pins
```
West boards per target: blackpill_f411ce, weact_g431=weact_stm32g431_core, weact_h562=weact_stm32h562_core, weact_h743=mini_stm32h743, rpi_pico, rpi_pico2=rpi_pico2/rp2350a/m33, promicro_nrf52840=promicro_nrf52840/nrf52840/uf2, esp32c3=esp32c3_devkitm, esp32s3=esp32s3_devkitc/esp32s3/procpu, ch32v003=ch32v003evt.

## Expected output
Prints `press the button (sw0) to toggle led0`; each press toggles the LED.

## Pin sources
Board DTS aliases sw0/led0 (zephyr/ws/zephyr/boards/weact/*, mini_stm32h743).
