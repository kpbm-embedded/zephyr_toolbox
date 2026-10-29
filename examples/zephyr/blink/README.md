# zephyr / blink

Blinks the on-board LED via the `led0` alias.

Status: builds; untested on hardware. Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`); per-board changes are devicetree overlays in `boards/`, nothing in the Zephyr tree is edited.

## Boards and pins
led0: BlackPill PC13 (active low), WeAct G431 PC6, WeAct H562 PB2, mini H743 PE3, Pico/Pico 2 GP25, Pro Micro nRF52840 P0.15, ESP32-C3 SuperMini GPIO8 via overlay (**UNVERIFIED**: GPIO8 as the SuperMini LED and its polarity vary between clones; adjust `boards/esp32c3_devkitm.overlay`).

## Build / flash / monitor
```
. ../../../toolchain/zephyr/env.sh
west build -p auto -b <west-board> -d $ZEPHYR_OUT/examples/blink/<west-board> .
../../../scripts/app example blink <west-board>   # the same build via this ID's scripts/app
west flash -d $ZEPHYR_OUT/examples/blink/<west-board>   # uf2 boards: copy zephyr/zephyr.uf2 to the bootloader drive
../../../scripts/app monitor /dev/ttyACM0 115200   # or /dev/ttyUSB0 with a USB-UART on the console pins
```
West boards per target: blackpill_f411ce, weact_g431=weact_stm32g431_core, weact_h562=weact_stm32h562_core, weact_h743=mini_stm32h743, rpi_pico, rpi_pico2=rpi_pico2/rp2350a/m33, promicro_nrf52840=promicro_nrf52840/nrf52840/uf2, esp32c3=esp32c3_devkitm, esp32s3=esp32s3_devkitc/esp32s3/procpu, ch32v003=ch32v003evt.

## Expected output
LED toggles every 500 ms; no console output.

## Pin sources
Board DTS in zephyr/ws/zephyr/boards (weact/blackpill_f411ce, weact/stm32g431_core, weact/stm32h562_core, weact/mini_stm32h743, raspberrypi/rpi_pico*, others/promicro_nrf52840). ESP32-C3 overlay is a guess, see above.
